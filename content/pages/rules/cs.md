---
title: Pravidla NEO
format: filtered_html
---

**Zavedení**

*Pokud není význam použitých pojmů jasný, podívejte se do **[glosáře](/glossary)**.*

**Rováš** je aplikace ekonomického systému **NEO**, ve kterém jsou účastníci odměňováni za jakoukoli aktivitu, kterou si zvolí nebo vymyslí. NEO má dva typy odměn. První je peněžní, udělovaná za pracovní úsilí, ve formě měny **chrony**. Uděluje se, když je pracovní zpráva podaná uživatelem NEO schválena dvěma dalšími uživateli. Vyplacená částka je úměrná odpracovanému času a všechny typy práce jsou odměňovány stejnou hodinovou sazbou. Chrony lze použít k úhradě zboží nebo služeb vyrobených v NEO nebo je směnit za národní měny.

Uživatelé NEO mohou také získat nepeněžní, nepřenosnou **Merity**, která je měřítkem hodnoty produktu práce. Kupující produktů a služeb neplatí výrobci, ale společnosti Rováše, která převádí zaplacené chrony na Merity, jeden chron za jeden Merit. Zaplacené chrony jsou následně zničeny a nové Merity jsou připsány prodávajícímu. Skóre Meritů je tedy odrazem schopnosti jednotlivce vytvářet společensky užitečné produkty nebo služby.

Oddělení Meritové odměny od ekonomické odměny vede k virtuální rovnosti příjmů, protože všichni vydělávají stejnou hodinovou mzdu v hodinách. Na druhou stranu, pracovníci NEO zůstávají motivováni k produkci inovativní práce, protože hodnotné produkty – a jejich tvůrci – jsou odměňováni nerovným množstvím Meritů, které odrážejí vytvořenou hodnotu.

**Základní struktura reportingu**

- Každý pracovní výkaz vyplněný v Rováši patří k **projektu** – entitě, která představuje vyrobený produkt nebo službu.
- Projekty mohou mít žádnou, jednu nebo více pracovních zpráv. Pokud projekt nemá žádnou pracovní zprávu, vlastník projektu nemůže být odměněn chronami. Stále však může získat Merity, pokud je jeho projekt zakoupen nebo odměněn.
- Výhody získané projektem si mohou sdílet **akcionáři** projektu. Schéma rozdělení výhod stanoví vlastník projektu nebo je mezi akcionáři vyjednáno.
- akcionáři projektu mohou být jednotlivci nebo jiné projekty,
- Projekty mohou být veřejné nebo soukromé. V soukromých projektech může pouze vlastník projektu přidávat nebo odebírat akcionáře a Merity lze mezi ně rozdělit podle pevného podílu nebo podle odpracovaného času. Veřejné projekty jsou otevřené komukoli a rozdělení Meritů je úměrné pracovní době účastníka projektu.

**Co je to práce**

Práce v NEO má následující atributy:

- je jakákoli činnost, která byla prokazatelně provedena. Důkazní břemeno leží na uživateli, který práci nahlašuje,
- výstup práce je k dispozici zdarma, nebo jej lze zakoupit za chronologické období,
- práce produkuje zboží nebo služby, které nejsou společensky škodlivé

**Reportingová práce**

Aby daná osoba obdržela standardních 10 chronů za každou odpracovanou hodinu, musí v Rováši vyplnit pracovní zprávu. Zpráva musí obsahovat následující informace:

- datum a čas zahájení práce (nejvýše 30 dní před podáním pracovního výkazu),
- délka odpracované doby,
- hlavní kategorie NACE, do které činnost spadá,
- důkaz, že nahlášená práce byla skutečně provedena. Poskytnutý důkaz závisí na typu provedené práce, nejčastěji se používá webová adresa a fotografie nebo videa zobrazující stav před a po provedení práce.

Rováš má také API, které umožňuje aplikacím třetích stran odesílat pracovní zprávy jménem uživatelů.

**Ověřování zpráv**

Poté, co uživatel odešle pracovní zprávu, Rováš vybere dva další uživatele a zašle jim e-mailové oznámení s žádostí o ověření zprávy. Ověřovatelé poté na základě důkazů uvedených ve zprávě a dalších informací poskytnutých společností Rováš zprávu buď schválí, nebo zamítnou. Pokud se oba ověřovatelé dohodnou na odlišném výsledku, Rováš určí a pozve dalšího – třetího ověřovatele. Po schválení Rováš autora okamžitě odmění bodovými body (chrony). Pokud uživatel-ověřovatel neověří zprávu do 5 dnů, Rováš uživatele penalizuje a vybere jiného ověřovatele.

**Tresty**

Pokud uživatel ověří požadavek, je k jeho veřejně viditelnému **skóre shody** připočten jeden bod. Pokud ověřovatel na žádosti o ověření nereaguje, je penalizován snížením skóre shody o jeden bod. Jeden bod shody je také odečten od skóre uživatele, pokud je vyplněný pracovní protokol zamítnut. Jakmile skóre klesne na mínus tři body, Rováš odečte deset bodů od skóre Meritů uživatele. Pokud skóre Meritů klesne pod nulu, jeho vlastník ztratí možnost vytvářet pracovní protokoly a získávat tak chrony. Jeho schopnost vytvářet projekty však zůstává, což mu umožňuje získávat Merity, a proto znovu podávat protokoly a získávat chrony, jakmile jeho skóre Meritů stoupne nad nulu.

Jakékoli rozhodnutí o ověření pracovní zprávy lze navíc napadnout zahájením formálního řízení zvaného **Spor**. Ve sporu žalobce popíše důvod stížnosti a požádá obžalovaného o nápravu problému. Pokud obžalovaný tak odmítne, obě strany shromáždí příznivce svého postoje. Po třech dnech Rováš spor uzavře a určí výsledek výpočtem kumulativního skóre uživatelů každé strany. Strana s největším počtem Meritů vyhrává. Postup při sporu a sankce jsou podrobněji popsány na [stránce Spory](/disputes).

**Druhy odměňované práce**

V NEO je jakákoli aktivita odměněna, pokud:

1. činnost byla prokazatelně provedena. Toto posouzení provádějí ověřovatelé, kteří své rozhodnutí zakládají převážně na dokladu o práci poskytnutém uživatelem podávajícím pracovní zprávu,
2. práce byla provedena po registraci uživatele v Rováši. Práce provedená předtím, než si uživatel v Rováši vytvořil účet, nemůže být odměněna,
3. práce začala nejvýše před 30 dny od podání pracovního výkazu. Práci se starším datem začátku nelze odměnit,
4. dílo produkuje zboží nebo služby, které jsou zdarma nebo je lze zakoupit za chrony,
5. Ověřovatelé se domnívají, že tato činnost není v rozporu s hodnotami komunity.

**Průhlednost**

Téměř všechny informace v NEO jsou volně přístupné jeho uživatelům. Výjimkou jsou osobní kontaktní údaje (e-mailová adresa) a údaje o bankovních účtech v jiných měnách než v chronech. To znamená, že každý uživatel si může kdykoli zkontrolovat pohyby na účtu jiného uživatele a také sledovat, jak a za co získal své chrony a Merity. Toto pravidlo transparentnosti je nezbytné pro zajištění stability ekonomiky NEO.

**Licence k používání dat**

Veškerá data dostupná v Rováš lze použít pouze k vytváření produktů a služeb, pro které existuje v Rováš projekt. Tvůrci takových projektů musí **[projekt Rováš](/node/1992)** přidat do svého projektu jako akcionáře a přidělit mu podíl odpovídající hodnotě, kterou data Rováš projektu poskytují.
