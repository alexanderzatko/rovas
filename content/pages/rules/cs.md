---
title: Pravidla NEO
format: filtered_html
---

**Úvod**

*Pokud není význam použitých pojmů jasný, podívejte se do **[glosáře](/glossary)**.*

**Rováš** je aplikace ekonomického systému **NEO**, ve kterém jsou účastníci odměňováni za jakoukoli aktivitu, kterou si zvolí nebo vymyslí. NEO má dva typy odměn. První je peněžní, udělovaná za pracovní úsilí, ve formě měny **chron**. Uděluje se, když je pracovní výkaz podaný uživatelem NEO schválen dvěma dalšími uživateli. Vyplacená částka je úměrná odpracovanému času a všechny typy práce jsou odměňovány stejnou hodinovou sazbou. Chrony lze použít k úhradě zboží nebo služeb vyrobených v NEO nebo je směnit za národní měny.

Uživatelé NEO mohou také získat nepeněžní, nepřenosnou **odměnu v Meritech**, která je měřítkem hodnoty produktu práce. Kupující produktů a služeb neplatí výrobci, ale Rováši, který převádí zaplacené chrony na Merity — jeden Merit za jeden chron. Zaplacené chrony jsou následně zničeny a nové Merity jsou připsány prodávajícímu. Skóre Meritů je tedy odrazem schopnosti jednotlivce vytvářet společensky užitečné produkty nebo služby.

Oddělení odměny v Meritech od peněžní odměny vede k virtuální rovnosti příjmů, protože všichni vydělávají stejnou hodinovou mzdu v chronech. Na druhou stranu pracovníci NEO zůstávají motivováni k produkci inovativní práce, protože hodnotné produkty — a jejich tvůrci — jsou odměňováni nerovným množstvím Meritů, které odrážejí vytvořenou hodnotu.

**Základní struktura podávání výkazů**

- Každý pracovní výkaz vyplněný v Rováši patří k **projektu** — entitě, která představuje vyrobený produkt nebo službu.
- Projekty mohou mít žádný, jeden nebo více pracovních výkazů. Pokud projekt nemá žádný pracovní výkaz, vlastník projektu nemůže být odměněn chrony. Stále však může získat Merity, pokud je jeho projekt zakoupen nebo odměněn.
- Merity získané projektem si mohou sdílet **podílníci** projektu. Schéma rozdělení Meritů stanoví vlastník projektu nebo je mezi podílníky vyjednáno.
- Podílníky projektu mohou být jednotlivci nebo jiné projekty.
- Projekty mohou být veřejné nebo soukromé. V soukromých projektech může pouze vlastník projektu přidávat nebo odebírat podílníky a Merity lze mezi ně rozdělit podle pevného podílu nebo podle odpracovaného času. Veřejné projekty jsou otevřené komukoli a rozdělení Meritů je úměrné pracovní době účastníka projektu.

**Co je to práce**

Práce v NEO má následující atributy:

- je jakákoli činnost, která byla prokazatelně provedena. Důkazní břemeno leží na uživateli, který práci nahlašuje,
- výstup práce je k dispozici zdarma, nebo jej lze zakoupit za chrony,
- práce produkuje zboží nebo služby, které nejsou společensky škodlivé.

**Podávání pracovních výkazů**

Aby daná osoba obdržela standardních 10 chronů za každou odpracovanou hodinu, musí v Rováši vyplnit pracovní výkaz. Výkaz musí obsahovat následující informace:

- datum a čas zahájení práce (nejvýše 30 dní před podáním pracovního výkazu),
- délka odpracované doby,
- hlavní kategorie NACE, do které činnost spadá,
- důkaz, že nahlášená práce byla skutečně provedena. Poskytnutý důkaz závisí na typu provedené práce; nejčastěji se používá webová adresa a fotografie nebo videa zobrazující stav před a po provedení práce.

Rováš má také API, které umožňuje aplikacím třetích stran odesílat pracovní výkazy jménem uživatelů.

**Ověřování výkazů**

Poté, co uživatel odešle pracovní výkaz, Rováš vybere dva další uživatele a zašle jim e-mailové oznámení s žádostí o ověření výkazu. Ověřovatelé poté na základě důkazů uvedených ve výkazu a dalších informací poskytnutých Rovášem výkaz buď schválí, nebo zamítnou. Pokud se oba ověřovatelé dohodnou na odlišném výsledku, Rováš určí a pozve dalšího — třetího ověřovatele. Po schválení Rováš autora okamžitě odmění chrony. Pokud uživatel-ověřovatel neověří výkaz do 5 dnů, Rováš uživatele penalizuje a vybere jiného ověřovatele.

**Důvěryhodní uživatelé**

Uživatelé, jejichž [skóre shody](/glossary#complianceScore) dosáhne alespoň 50, nebo kteří vlastní projekt, z jehož odměn získali více než 500 Meritů, se stávají **[důvěryhodnými uživateli](/glossary#trustedUser)**. Mohou si zvolit okamžité schválení svých pracovních výkazů bez peer ověření a omezit nebo zastavit přijímání žádostí o ověření. Tyto preference se nastavují v profilu. Okamžitě schválené výkazy lze stále napadnout sporem; prohra ve sporu může status důvěryhodného uživatele odejmout, pokud již nesplňuje podmínky.

**Tresty**

Pokud uživatel ověří požadavek, je k jeho veřejně viditelnému **skóre shody** připočten jeden bod. Pokud ověřovatel na žádost o ověření nereaguje, je penalizován snížením skóre shody o jeden bod. Jeden bod shody je také odečten od skóre uživatele, pokud je vyplněný pracovní výkaz zamítnut. Jakmile skóre klesne na mínus tři body, Rováš odečte deset Meritů od skóre Meritů uživatele. Pokud skóre Meritů klesne pod nulu, jeho vlastník ztratí možnost vytvářet pracovní výkazy a získávat tak chrony. Jeho schopnost vytvářet projekty však zůstává, což mu umožňuje získávat Merity, a proto znovu podávat výkazy a získávat chrony, jakmile jeho skóre Meritů stoupne nad nulu.

Jakékoli rozhodnutí o ověření pracovního výkazu lze navíc napadnout zahájením formálního řízení zvaného **Spor**. Ve sporu žalobce popíše důvod stížnosti a požádá obžalovaného o nápravu problému. Pokud obžalovaný tak odmítne, obě strany shromáždí příznivce svého postoje. Po třech dnech Rováš spor uzavře a určí výsledek výpočtem kumulativního skóre Meritů uživatelů každé strany. Strana s největším počtem Meritů vyhrává. Postup při sporu a sankce jsou podrobněji popsány na [stránce Spory](/disputes).

**Druhy odměňované práce**

V NEO je jakákoli aktivita odměněna, pokud:

1. činnost byla prokazatelně provedena. Toto posouzení provádějí ověřovatelé, kteří své rozhodnutí zakládají převážně na dokladu o práci poskytnutém uživatelem podávajícím pracovní výkaz,
2. práce byla provedena po registraci uživatele v Rováši. Práce provedená předtím, než si uživatel v Rováši vytvořil účet, nemůže být odměněna,
3. práce začala nejvýše před 30 dny od podání pracovního výkazu. Práci se starším datem začátku nelze odměnit,
4. práce produkuje zboží nebo služby, které jsou zdarma nebo je lze zakoupit za chrony,
5. ověřovatelé se domnívají, že tato činnost není v rozporu s hodnotami komunity.

**Transparentnost**

Téměř všechny informace v NEO jsou volně přístupné jeho uživatelům. Výjimkou jsou osobní kontaktní údaje (e-mailová adresa) a údaje o bankovních účtech v jiných měnách než v chronech. To znamená, že každý uživatel si může kdykoli zkontrolovat pohyby na účtu jiného uživatele a také sledovat, jak a za co získal své chrony a Merity. Toto pravidlo transparentnosti je nezbytné pro zajištění stability ekonomiky NEO.

**Licence k používání dat**

Veškerá data dostupná v Rováši lze použít pouze k vytváření produktů a služeb, pro které existuje v Rováši projekt. Tvůrci takových projektů musí **[projekt Rováš](/node/1992)** přidat do svého projektu jako podílníka a přidělit mu podíl odpovídající hodnotě, kterou data Rováš projektu poskytují.
