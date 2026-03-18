# Reward project: Card payment Rule vs PHP fee processing

This document extracts what **`rules_process_card_payment_orders`** does for **`field_payer_type = project`** (reward project) and compares it to **`_rovas_process_stripe_fees_for_order()`** in the Stripe webhook, including duplication and race conditions.

---

## 1. When each path runs

| Path | Trigger | When |
|------|--------|------|
| **Rule** `rules_process_card_payment_orders` | Event `commerce_payment_order_paid_in_full` | After Commerce records payment and order balance is zero (e.g. after Stripe payment form success or after webhook creates transaction). |
| **PHP** `_rovas_process_stripe_fees_for_order()` | Called from webhook only | Inside `rovas_stripe_webhooks_process_payment_intent_succeeded()` after finding/creating the order and (for express) after `commerce_checkout_complete`. |

---

## 2. Rule: `rules_process_card_payment_orders` (project case only)

**Runs on:** `commerce_payment_order_paid_in_full`  
**Conditions:** order has `field_payment_recipient`, payment method is **not** `bank_transfer`.

**Shared steps (all card orders with recipient):**

1. `charge_fees = TRUE`, read `order_total` from order.
2. `rovas_forex_transaction_params(eur_to_chr, order_total, processor_fee=0, charge_fees=TRUE)` → eur_basket, chr_basket, output_amount, fee_amount, etc.
3. **`component_rules_process_forex_order`** with:
   - `order_id`, `payment_status=0`, `euro_basket`, `chron_basket`, `conv_type=euros_to_chrons`, `input_amt`, `output_amt`, `fee`, `user_uid`
   - **Creates `forex_transaction` node and sets `forex_node.field_order_record` = commerce order.**
4. `commerce_invoice_receipt_action_mail`.
5. If order contains product **time-share**: process time-share, update forex baskets (php_eval), call **`rules_process_recipient_order_core`**.
6. **SWITCH** on `commerce-order:field-payer-type`:

**CASE `project` (reward project):**

- Add variable `email` = `commerce-order:mail`.
- `entity_fetch` node `commerce-order:field-payment-recipient` → `rewarded_node`.
- **php_eval:**  
  If order is **not** express checkout (`$order->data['stripe_pi']['payment_intent']['metadata']['context'] != 'express_checkout'`), call  
  **`rovas_project_reward_notify_purchase($rewarded_node, $order_total, 'EUR', $email)`**.

So for **project** the Rule does **not** call `rules_process_recipient_order_core` and does **not** update forex baskets; it only:

- Creates the forex node (via `rules_process_forex_order`).
- Sends the invoice mail.
- Optionally notifies purchase (and explicitly skips that for express checkout).

For **time-share** and **user+merits**, the Rule does call `rules_process_recipient_order_core` and updates baskets.

---

## 3. PHP: `_rovas_process_stripe_fees_for_order()`

**Called from:** `_rovas_stripe_webhooks_add_balance_fees_and_process()` in the webhook (both express and regular card when order is found).

**Conditions (early exit):**

- Payer type is **user** → return FALSE.
- Order contains product **time-share** → return FALSE.

**Assumptions:**

- A **`forex_transaction`** node already exists with **`field_order_record`** = this order (EntityFieldQuery by `field_order_record` → order_id).

**Actions:**

1. Load forex node; skip if `field_payment_status == 1`.
2. Compute Rovas fee, merit allocation, `merit_fee`, `net_after_merit`, `total_fee` from Stripe balance transaction (`fee_amount`, `net_amount`).
3. If `net_after_merit > 0`: **`process_preferred_allocation_payment($rewarded_project, $net_after_merit)`**.
4. **`rovas_forex_transaction_params('eur_to_chr', $merit_fee, $fee_amount, $charge_fees=FALSE)`** (uses **real** Stripe fee).
5. **`rules_invoke_component('rules_process_recipient_order_core', $output_amount, 1, $order)`** (creates receipt, updates project merits, runs allocation). Parameter order must match `USES VARIABLES`: `converted_chrons_paid` (decimal), `payment_status` (integer), `order` (commerce_order).
6. Update **forex baskets** (variable_set `forex_baskets` / `forex_baskets_temp`).
7. Update **forex node** fields (baskets, input_amount, conversion_fee, output_amount, **field_payment_status = 1**), save.

So the PHP path:

- **Depends on** the forex node (and its link to the order) **already existing**.
- Uses **Stripe's** fee/net and updates the **same** forex node with final amounts and payment_status.
- Is the only place that runs **`rules_process_recipient_order_core`** for **plain project** (non–time-share) card orders.

---

## 4. Who creates the forex node and when

| Flow | Who creates forex node | When |
|------|-------------------------|------|
| **Card (checkout on site)** | Rule `rules_process_card_payment_orders` | On `commerce_payment_order_paid_in_full` (after payment is recorded). |
| **Express (webhook)** | No Rule runs that creates it | Only `commerce_checkout_complete` is fired; no Rule on that event creates a forex node for card. So **no one creates it**. |
| **Bank** | Rule `rules_process_bank_payment_orders` | On `commerce_checkout_complete` (with payment_method = bank_transfer). |

So:

- **Express card:** The webhook creates the order and calls `_rovas_process_stripe_fees_for_order()`, but **no forex node is ever created** for that order → "Forex node not found" and fee/recipient processing never run.
- **Regular card:** The forex node is created by the Rule when **`commerce_payment_order_paid_in_full`** runs. That event is invoked **after** `_rovas_stripe_webhooks_add_balance_fees_and_process()` in the webhook (see processor.inc: 109 then 114–120). So in the webhook, **fee processing runs first**, then **paid_in_full**. When fee processing runs, the Rule has not run yet → **forex node does not exist yet** → same "Forex node not found" in the webhook for regular card as well (unless paid_in_full was already fired earlier by the checkout UI).

---

## 5. Duplication and race conditions

### 5.1 Race: webhook vs Rule (regular card)

- **Webhook order:** find order → **fee processing** (expects forex node) → then invoke **paid_in_full** → Rule runs and **creates** forex node.
- So in the webhook, **`_rovas_process_stripe_fees_for_order()` always runs before the Rule has created the forex node** for that request. Either:
  - Fee processing logs "Forex node not found" and does nothing, and later when/if `commerce_payment_order_paid_in_full` is triggered (e.g. from checkout page), the Rule creates the node and does **not** run recipient core or update the node with Stripe fees; or
  - `paid_in_full` is sometimes fired from the front-end first (e.g. after PaymentIntent confirm on checkout), the Rule creates the forex node, and then the webhook runs and fee processing finds the node and updates it.

So there is a **race** between "checkout UI / payment completion" and "webhook". If the webhook runs first, fee processing never sees a forex node for that order in that request.

### 5.2 Express: forex node never created

For express checkout, the webhook:

1. Creates the order.
2. Creates the transaction.
3. Fires **only** `commerce_checkout_complete` (not `commerce_payment_order_paid_in_full` in the same path).
4. Runs fee processing.

No Rule on `commerce_checkout_complete` creates a forex node for card. So the forex node is **never** created for express orders → **`_rovas_process_stripe_fees_for_order()` always fails** for express (Forex node not found), and **`rules_process_recipient_order_core`** is never run for those orders.

### 5.3 Duplication / split responsibility (project, non–time-share)

- **Rule (project case):** Creates forex node, sends invoice, optionally `rovas_project_reward_notify_purchase`. Does **not** call `rules_process_recipient_order_core` for project. Does **not** use Stripe fee or update forex node with final fee/status.
- **PHP fee processing:** Expects forex node to exist; uses Stripe fee; updates forex node (baskets, payment_status, etc.); calls **`rules_process_recipient_order_core`** (receipt, merits, allocation).

So for a **plain project** reward (subs-paymnt, no time-share):

- **Recipient processing** (receipt, merits, allocation) is intended to run only in **PHP** (`_rovas_process_stripe_fees_for_order`), not in the Rule.
- **Forex node creation** is in the **Rule** (on paid_in_full); **forex node update** (Stripe fee, status) is in **PHP** (webhook). So they are split across two paths and two triggers, and the PHP path depends on the node already existing when the webhook runs.

---

## 6. Summary table (project reward, card)

| Step | Rule (paid_in_full) | PHP (webhook) |
|------|----------------------|----------------|
| Create forex node | Yes (`rules_process_forex_order`) | No (expects it) |
| Set field_order_record | Yes (in component) | — |
| Invoice mail | Yes | No |
| Notify purchase (callback) | Yes, unless express | No |
| Use Stripe fee / net | No (processor_fee=0 in Rule) | Yes |
| Update forex node (fee, status, baskets) | No | Yes |
| `rules_process_recipient_order_core` | No (for project) | Yes |

**Conclusion:** For project card payments, the Rule creates the forex node and does notification; the PHP path is supposed to add Stripe fee data and run recipient core, but it only works if the forex node already exists when the webhook runs. For **express**, the node is never created, so PHP always fails. For **regular card**, ordering in the webhook (fee processing before paid_in_full) causes a race and often "Forex node not found."

---

## 7. Recommended direction (for future changes)

1. **Express:** Ensure a forex node exists for the order before calling `_rovas_process_stripe_fees_for_order()` in the webhook (e.g. create it in the webhook after creating the order, or fire an event that a Rule can react to and create the node with `field_order_record`).
2. **Regular card:** In the webhook, either:
   - Invoke **`commerce_payment_order_paid_in_full`** first (so the Rule creates the forex node), then call `_rovas_stripe_webhooks_add_balance_fees_and_process()`, or
   - Create the forex node in PHP when it is missing (e.g. in the webhook or inside `_rovas_process_stripe_fees_for_order`) so fee processing and recipient core always have a node to work with.
3. **Document** that for project (non–time-share) card orders, recipient core is run only from PHP fee processing, and that the forex node must exist before that runs.

---

## 8. Required Drupal Rules for rewpro order processing

The following Rules configurations (stored in the database) are required for the reward-project (rewpro) flow. Export them via **Configuration > Workflow > Rules** or Drush `rules-export` and version them (e.g. in a custom module’s `hook_default_rules_configuration()` or Features) so they are deployable.

**Reaction rules (on events):**

| Rule name | Event | Purpose |
|-----------|--------|--------|
| `rules_process_bank_payment_orders` | `commerce_checkout_complete` | Bank transfer: create forex node, set order to processing, send invoice. |
| `rules_process_card_payment_orders` | `commerce_payment_order_paid_in_full` | Card: create forex node, send invoice, notify purchase (project case); time-share/user+merits run recipient core. |
| `rules_change_status_for_paid_bank_transfer_orders` | `commerce_payment_order_paid_in_full` | Bank: set order status to completed when paid. |
| `commerce_checkout_order_convert` | `commerce_checkout_complete` | Assign anonymous order to existing user by email. |

**Components (invoked by code or other rules):**

| Component name | Purpose |
|----------------|--------|
| `rules_process_forex_order` | Creates `forex_transaction` node and sets `field_order_record` to the commerce order; used by card/bank rules and by `_rovas_project_reward_ensure_forex_node_for_order()` for express. |
| `rules_process_recipient_order_core` | Creates purchase receipt, updates project merits, runs allocation; invoked from PHP `_rovas_process_stripe_fees_for_order()` and from card rule (time-share/user+merits). |
| `rules_chrons_actually_available` | Returns Chron balance for a user; used by rewpro form. |

To export from Drush:  
`drush rules-export RULE_NAME`  
(e.g. `rules_process_forex_order`, `rules_process_recipient_order_core`, `rules_process_card_payment_orders`, `rules_process_bank_payment_orders`).
