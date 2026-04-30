#!/usr/bin/env python3
"""
Long-running PO translation helper with progress reporting and checkpoints.

Designed for large files where translation can take a long time and must be observable.

Examples:
  # Translate Czech file from Slovak source translations (best for your case)
  python3 scripts/po_translate_helper.py \
    --target-po translations/po/cs.default.po \
    --source-po translations/po/sk.default.po \
    --from-lang sk --to-lang cs \
    --mode source-msgstr

  # Re-translate only suspicious Slovak leftovers in target msgstr
  python3 scripts/po_translate_helper.py \
    --target-po translations/po/cs.default.po \
    --from-lang sk --to-lang cs \
    --mode suspicious-only
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple

try:
    import polib
except ImportError as exc:  # pragma: no cover
    raise SystemExit(
        "Missing dependency: polib\n"
        "Install in a virtualenv, e.g.:\n"
        "  python3 -m venv .venv-i18n\n"
        "  .venv-i18n/bin/pip install polib requests"
    ) from exc

try:
    import requests
except ImportError as exc:  # pragma: no cover
    raise SystemExit(
        "Missing dependency: requests\n"
        "Install in a virtualenv, e.g.:\n"
        "  python3 -m venv .venv-i18n\n"
        "  .venv-i18n/bin/pip install polib requests"
    ) from exc


TOKEN_RE = re.compile(
    r"(\[[^\]\n]+\]|%\w|@[A-Za-z_][\w-]*|!\w|<[^>]+>|\{[^{}\n]+\})"
)
SLOVAK_HINT_RE = re.compile(r"[ďťľôäĺŕ]|skúste|prihlás|zrušiť|uložiť", re.IGNORECASE)
TAG_RE = re.compile(r"</?([a-zA-Z0-9]+)(?:\s[^>]*)?>")
ALLOWED_DEFAULT_TAGS: Set[str] = {
    "a", "abbr", "acronym", "address", "b", "bdo", "big", "blockquote", "br",
    "caption", "cite", "code", "col", "colgroup", "dd", "del", "dfn", "dl", "dt",
    "em", "h1", "h2", "h3", "h4", "h5", "h6", "hr", "i", "ins", "kbd", "li", "ol",
    "p", "pre", "q", "samp", "small", "span", "strong", "sub", "sup", "table",
    "tbody", "td", "tfoot", "th", "thead", "tr", "tt", "ul", "var",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Translate and normalize PO msgstr values.")
    parser.add_argument("--target-po", required=True, help="PO file to modify.")
    parser.add_argument(
        "--source-po",
        help="Optional source PO for lookup by key (msgctxt,msgid,msgid_plural).",
    )
    parser.add_argument("--from-lang", default="sk", help="Source language code.")
    parser.add_argument("--to-lang", default="cs", help="Target language code.")
    parser.add_argument(
        "--mode",
        choices=["source-msgstr", "empty-only", "suspicious-only", "force-all", "sanitize-default-html"],
        default="source-msgstr",
        help=(
            "source-msgstr: translate from source-po msgstr when available; "
            "empty-only: fill only empty target msgstr from msgid; "
            "suspicious-only: retranslate only target msgstr that look Slovak; "
            "force-all: translate all non-empty target msgstr; "
            "sanitize-default-html: do not translate, only make msgstr markup safe for default textgroup import."
        ),
    )
    parser.add_argument("--save-every", type=int, default=200, help="Save after N processed entries.")
    parser.add_argument(
        "--report-every-seconds",
        type=int,
        default=8,
        help="Progress reporting interval in seconds.",
    )
    parser.add_argument("--request-timeout", type=float, default=10.0)
    parser.add_argument("--max-retries", type=int, default=4)
    parser.add_argument(
        "--state-file",
        help="Optional JSON state file for progress stats (written on each save).",
    )
    return parser.parse_args()


def protect_tokens(text: str) -> Tuple[str, List[str]]:
    tokens: List[str] = []

    def _repl(match: re.Match[str]) -> str:
        idx = len(tokens)
        tokens.append(match.group(0))
        return f"__TOK{idx}__"

    return TOKEN_RE.sub(_repl, text), tokens


def unprotect_tokens(text: str, tokens: List[str]) -> str:
    for i, token in enumerate(tokens):
        text = text.replace(f"__TOK{i}__", token)
    return text


def translate_text(
    text: str,
    from_lang: str,
    to_lang: str,
    timeout_s: float,
    retries: int,
    cache: Dict[str, str],
) -> str:
    if text in cache:
        return cache[text]

    url = "https://translate.googleapis.com/translate_a/single"
    params = {"client": "gtx", "sl": from_lang, "tl": to_lang, "dt": "t", "q": text}
    translated = text

    for attempt in range(retries):
        try:
            response = requests.get(url, params=params, timeout=timeout_s)
            response.raise_for_status()
            payload = response.json()
            translated = "".join(item[0] for item in payload[0] if item and item[0] is not None)
            if translated:
                break
        except Exception:
            time.sleep(0.35 * (attempt + 1))

    cache[text] = translated
    return translated


def entry_key(entry: "polib.POEntry") -> Tuple[str, str, str]:
    return (entry.msgctxt or "", entry.msgid or "", entry.msgid_plural or "")


def should_process_entry(mode: str, current_msgstr: str) -> bool:
    clean = current_msgstr.strip()
    if mode == "empty-only":
        return clean == ""
    if mode == "suspicious-only":
        return clean != "" and bool(SLOVAK_HINT_RE.search(clean))
    if mode == "force-all":
        return clean != ""
    if mode == "sanitize-default-html":
        return clean != ""
    # source-msgstr
    return True


def sanitize_default_html(msgid: str, msgstr: str) -> str:
    """
    Make translation markup safer for Drupal default textgroup import.
    - remove img/script/style/iframe tags
    - convert div wrappers to span
    - keep only tags allowed by locale_string_is_safe() list
    - if msgid has no tags, strip all tags from msgstr
    """
    out = msgstr

    # Remove known problematic tags first.
    out = re.sub(r"<\s*(img|script|style|iframe)\b[^>]*?/?>", "", out, flags=re.IGNORECASE)

    # Normalize common layout wrapper to inline-safe span.
    out = re.sub(r"<\s*div\b[^>]*>", "<span>", out, flags=re.IGNORECASE)
    out = re.sub(r"<\s*/\s*div\s*>", "</span>", out, flags=re.IGNORECASE)

    msgid_tags = {m.group(1).lower() for m in TAG_RE.finditer(msgid)}
    if not msgid_tags:
        # If source string is plain text, force plain text in translation too.
        out = re.sub(r"</?[^>]+>", "", out)
        return out.strip()

    def _tag_filter(match: re.Match[str]) -> str:
        full = match.group(0)
        tag = match.group(1).lower()
        if tag not in ALLOWED_DEFAULT_TAGS:
            return ""
        # Keep only tags that also appear in source (except harmless <br>).
        if tag != "br" and tag not in msgid_tags:
            return ""
        return full

    out = TAG_RE.sub(_tag_filter, out)
    # Collapse whitespace around removed tags.
    out = re.sub(r"[ \t]{2,}", " ", out)
    out = re.sub(r"\n{3,}", "\n\n", out)
    return out.strip()


def maybe_translate_value(
    source_text: str,
    from_lang: str,
    to_lang: str,
    timeout_s: float,
    retries: int,
    cache: Dict[str, str],
) -> str:
    protected, tokens = protect_tokens(source_text)
    translated = translate_text(protected, from_lang, to_lang, timeout_s, retries, cache)
    return unprotect_tokens(translated, tokens)


def main() -> int:
    args = parse_args()

    target_path = Path(args.target_po)
    if not target_path.exists():
        raise SystemExit(f"Target PO not found: {target_path}")

    source_po = polib.pofile(str(args.source_po)) if args.source_po else None
    source_index = {entry_key(e): e for e in source_po} if source_po else {}

    target_po = polib.pofile(str(target_path))
    total = len(target_po)
    cache: Dict[str, str] = {}

    changed_entries = 0
    changed_values = 0
    skipped = 0
    errors = 0
    processed = 0
    saved = 0

    start = time.time()
    last_report = start

    for idx, entry in enumerate(target_po):
        processed += 1
        entry_changed = False

        # Singular msgstr
        source_text_singular: Optional[str] = None
        if args.mode == "source-msgstr" and source_po:
            source_entry = source_index.get(entry_key(entry))
            if source_entry and source_entry.msgstr.strip():
                source_text_singular = source_entry.msgstr
        elif args.mode == "empty-only":
            source_text_singular = entry.msgid or ""
        elif args.mode == "sanitize-default-html":
            source_text_singular = entry.msgstr
        else:
            source_text_singular = entry.msgstr

        if source_text_singular is None or source_text_singular.strip() == "":
            skipped += 1
        else:
            if should_process_entry(args.mode, entry.msgstr):
                try:
                    if args.mode == "sanitize-default-html":
                        new_value = sanitize_default_html(entry.msgid or "", source_text_singular)
                    else:
                        new_value = maybe_translate_value(
                            source_text_singular,
                            args.from_lang,
                            args.to_lang,
                            args.request_timeout,
                            args.max_retries,
                            cache,
                        )
                    if new_value and new_value != entry.msgstr:
                        entry.msgstr = new_value
                        changed_values += 1
                        entry_changed = True
                except Exception:
                    errors += 1
            else:
                skipped += 1

        # Plural msgstr[n]
        if entry.msgstr_plural:
            for plural_key, plural_val in list(entry.msgstr_plural.items()):
                source_plural: Optional[str] = None
                if args.mode == "source-msgstr" and source_po:
                    source_entry = source_index.get(entry_key(entry))
                    if source_entry and source_entry.msgstr_plural.get(plural_key, "").strip():
                        source_plural = source_entry.msgstr_plural.get(plural_key, "")
                elif args.mode == "empty-only":
                    source_plural = entry.msgid_plural or entry.msgid or ""
                elif args.mode == "sanitize-default-html":
                    source_plural = plural_val
                else:
                    source_plural = plural_val

                if source_plural is None or source_plural.strip() == "":
                    continue

                if not should_process_entry(args.mode, plural_val):
                    continue

                try:
                    if args.mode == "sanitize-default-html":
                        new_plural = sanitize_default_html(entry.msgid_plural or entry.msgid or "", source_plural)
                    else:
                        new_plural = maybe_translate_value(
                            source_plural,
                            args.from_lang,
                            args.to_lang,
                            args.request_timeout,
                            args.max_retries,
                            cache,
                        )
                    if new_plural and new_plural != plural_val:
                        entry.msgstr_plural[plural_key] = new_plural
                        changed_values += 1
                        entry_changed = True
                except Exception:
                    errors += 1

        if entry_changed:
            changed_entries += 1

        now = time.time()
        if processed % args.save_every == 0:
            target_po.save(str(target_path))
            saved += 1
            if args.state_file:
                state = {
                    "target_po": str(target_path),
                    "processed": processed,
                    "total": total,
                    "changed_entries": changed_entries,
                    "changed_values": changed_values,
                    "skipped": skipped,
                    "errors": errors,
                    "saves": saved,
                    "elapsed_seconds": round(now - start, 2),
                }
                Path(args.state_file).write_text(json.dumps(state, indent=2), encoding="utf-8")

        if now - last_report >= args.report_every_seconds:
            pct = (processed / max(total, 1)) * 100.0
            rate = processed / max(now - start, 0.001)
            eta = (total - processed) / max(rate, 0.001)
            print(
                f"[progress] {processed}/{total} ({pct:.1f}%) "
                f"changed_entries={changed_entries} changed_values={changed_values} "
                f"skipped={skipped} errors={errors} saves={saved} "
                f"rate={rate:.2f} entries/s eta={eta/60:.1f}m",
                flush=True,
            )
            last_report = now

    target_po.save(str(target_path))
    elapsed = time.time() - start
    print(
        f"[done] processed={processed} total={total} "
        f"changed_entries={changed_entries} changed_values={changed_values} "
        f"skipped={skipped} errors={errors} saves={saved} elapsed={elapsed:.1f}s",
        flush=True,
    )
    return 0 if errors == 0 else 2


if __name__ == "__main__":
    sys.exit(main())
