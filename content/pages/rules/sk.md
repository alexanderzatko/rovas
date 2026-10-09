---
title: Pravidlá NEO
format: filtered_html
---

**Úvod**

*Ak nie je význam použitých pojmov jasný, pozrite si **[glosár](/glossary)**.*

**Rováš** je aplikácia ekonomického systému **NEO**, v ktorom sú účastníci odmeňovaní za akúkoľvek aktivitu, ktorú si zvolia alebo vymyslia. NEO má dva typy odmien. Jedna je peňažná, udeľovaná za pracovné úsilie, vo forme meny **chron**. Udeľuje sa, keď je pracovná správa podaná používateľom NEO schválená dvoma ďalšími používateľmi. Vyplatená suma peňazí je úmerná odpracovanému času a všetky typy práce sú odmeňované rovnakou hodinovou sadzbou. Chrony je možné použiť na platbu za tovar alebo služby vyrobené v NEO alebo ich vymeniť za národné meny.

Používatelia NEO môžu tiež získať nepeňažnú, neprenosnú **Meritovú odmenu**, ktorá je mierou hodnoty produktu práce. Kupujúci produktov a služieb neplatia výrobcovi, ale spoločnosti Rováša, ktorá premieňa zaplatené chrony na Merity, jeden chron za jeden Merit. Zaplatené chrony sú následne zničené a nové Merity sú pridelené predajcovi. Skóre Meritov je teda odrazom schopnosti jednotlivca vytvárať spoločensky užitočné produkty alebo služby.

Oddelenie Meritovej odmeny od ekonomickej odmeny vedie k virtuálnej rovnosti príjmov, keďže každý zarába rovnakú hodinovú mzdu v hodinách. Na druhej strane, pracovníci NEO zostávajú motivovaní produkovať inovatívnu prácu, pretože hodnotné produkty – a ich tvorcovia – sú odmeňovaní nerovným množstvom Meritov, ktoré odrážajú vyprodukovanú hodnotu.

**Základná štruktúra podávania správ**

- Každá pracovná správa vyplnená v Rováši patrí k **projektu** – entite, ktorá predstavuje vyrobený tovar alebo službu.
- Projekty môžu mať žiadnu, jednu alebo viac pracovných správ. Ak projekt nemá žiadnu pracovnú správu, vlastník projektu nemôže byť odmenený chronami. Stále však môže získať Merity, ak je jeho projekt zakúpený alebo odmenený.
- Výhody získané projektom si môžu rozdeliť **akcionári** projektu. Schému rozdelenia výhod stanovuje vlastník projektu alebo sa o nej dohodnú akcionári.
- akcionármi projektu môžu byť jednotlivci alebo iné projekty,
- Projekty môžu byť verejné alebo súkromné. V súkromných projektoch môže iba vlastník projektu pridávať alebo odoberať akcionárov a Merity sa medzi nimi môžu rozdeliť podľa pevného podielu alebo podľa odpracovaného času. Verejné projekty sú otvorené pre každého a rozdelenie Meritov je úmerné pracovnému času účastníka projektu.

**Čo je práca**

Práca v NEO má nasledujúce atribúty:

- je akákoľvek činnosť, ktorá bola preukázateľne vykonaná. Dôkazné bremeno je na používateľovi, ktorý prácu nahlasuje,
- výstup práce je dostupný zadarmo alebo sa dá zakúpiť za určité časy,
- práca produkuje tovary alebo služby, ktoré nie sú spoločensky škodlivé

**Podávanie správ o práci**

Aby osoba mohla získať štandardných 10 chronov za každú odpracovanú hodinu, musí v Rováši vyplniť pracovný výkaz. Výkaz musí obsahovať nasledujúce informácie:

- dátum a čas začiatku práce,
- dĺžka odpracovaného času,
- hlavná kategória NACE, do ktorej činnosť patrí,
- dôkaz o tom, že nahlásená práca bola skutočne vykonaná. Poskytnutý dôkaz závisí od typu vykonanej práce, pričom najčastejšie sa používa webová adresa a fotografie alebo videá zobrazujúce stav pred a po vykonaní práce.

Rováš má tiež API, ktoré umožňuje aplikáciám tretích strán odosielať pracovné správy v mene používateľov.

**Overovanie správ**

Po odoslaní pracovnej správy používateľom spoločnosť Rováš vyberie dvoch ďalších používateľov a pošle im e-mailové upozornenie so žiadosťou o overenie správy. Overovatelia potom použijú dôkazy uvedené v správe a ďalšie informácie poskytnuté spoločnosťou Rováš na schválenie alebo zamietnutie správy. Ak dvaja overovatelia vydajú odlišné rozhodnutia, spoločnosť Rováš určí a pozve ďalšieho – tretieho overovateľa. Po schválení spoločnosť Rováš okamžite odmení autora chronami. Ak overovateľ neoverí správu do 5 dní, spoločnosť Rováš používateľa penalizuje a vyberie iného overovateľa.

**Tresty**

Ak používateľ overí žiadosť, k jeho verejne viditeľnému **skóre súladu** sa pripočíta jeden bod. Ak overovateľ na žiadosti o overenie nereaguje, je penalizovaný znížením skóre súladu o jeden bod. Jeden bod súladu sa tiež odpočíta od skóre používateľa, ak je zamietnutý pracovný výkaz, ktorý vyplnil. Keď skóre klesne na mínus tri body, Rováš odpočíta desať Meritov od Meritového skóre používateľa. Ak Meritové skóre klesne pod nulu, jeho držiteľ stráca možnosť vytvárať pracovné výkazy a získavať tak chrony. Jeho schopnosť vytvárať projekty však zostáva zachovaná, čo mu umožňuje získavať Merity, a preto opäť podávať výkazy a získavať chrony, keď jeho Meritové skóre stúpne nad nulu.

Akékoľvek rozhodnutie o overení pracovnej správy možno dodatočne napadnúť začatím formálneho konania s názvom **Spor**. V spore žalobca popíše dôvod sťažnosti a požiada obvineného o nápravu problému. Ak obvinený odmietne tak urobiť, obe strany zhromaždia podporovateľov svojho stanoviska. Po troch dňoch spoločnosť Rováš spor uzavrie a určí výsledok výpočtom kumulatívneho Meritového skóre používateľov každej strany. Vyhráva strana s najväčším počtom Meritov. Postup pri spore a sankcie sú podrobnejšie opísané na [stránke Spory](/disputes).

**Druhy odmeňovanej práce**

V NEO je akákoľvek aktivita odmenená, ak:

1. činnosť bola preukázateľne vykonaná. Toto posúdenie robia overovatelia, ktorí svoje rozhodnutie zakladajú najmä na dôkaze o práci poskytnutom používateľom podávajúcim pracovnú správu,
2. práca bola vykonaná po registrácii používateľa v Rováši. Práca vykonaná predtým, ako si používateľ vytvoril účet v Rováši, nemôže byť odmenená,
3. dielo produkuje tovary alebo služby, ktoré sú bezplatné alebo sa dajú kúpiť za chrony,
4. Overovatelia sa domnievajú, že činnosť nie je v rozpore s hodnotami komunity.

**Transparentnosť**

Takmer všetky informácie v NEO sú voľne prístupné jeho používateľom. Výnimkou sú osobné kontaktné údaje (e-mailová adresa) a údaje o bankových účtoch v iných menách okrem chronu. To znamená, že každý používateľ si môže kedykoľvek skontrolovať pohyby na účte iného používateľa, ako aj sledovať, ako a za čo získal svoje chrony a Merity. Toto pravidlo transparentnosti je potrebné na zabezpečenie stability ekonomiky NEO.

**Licencia na používanie údajov**

Akékoľvek údaje dostupné v Rováši sa môžu použiť iba na vytváranie produktov a služieb, pre ktoré existuje v Rováši projekt. Tvorcovia takýchto projektov musia pridať **[projekt Rováš](/node/1992)** do svojho projektu ako akcionári a prideliť mu podiel zodpovedajúci hodnote, ktorú údaje Rováš poskytujú projektu.
