---
title: Pravidlá NEO
format: filtered_html
---

**Úvod**

*Ak nie je význam použitých pojmov jasný, pozrite si **[glosár](/glossary)**.*

**Rováš** je aplikácia ekonomického systému **NEO**, v ktorom sú účastníci odmeňovaní za akúkoľvek aktivitu, ktorú si zvolia alebo vymyslia. NEO má dva typy odmien. Jedna je peňažná, udeľovaná za pracovné úsilie, vo forme meny **chron**. Udeľuje sa, keď je pracovný výkaz podaný používateľom NEO schválený dvoma ďalšími používateľmi. Vyplatená suma peňazí je úmerná odpracovanému času a všetky typy práce sú odmeňované rovnakou hodinovou sadzbou. Chrony je možné použiť na platbu za tovar alebo služby vyrobené v NEO alebo ich vymeniť za národné meny.

Používatelia NEO môžu tiež získať nepeňažnú, neprenosnú **odmenu v meritoch**, ktorá je mierou hodnoty produktu práce. Kupujúci produktov a služieb neplatia výrobcovi, ale Rovášu, ktorý premieňa zaplatené chrony na merity — jeden Merit za jeden chron. Zaplatené chrony sú následne zničené a nové merity sú pridelené predajcovi. Skóre meritov je teda odrazom schopnosti jednotlivca vytvárať spoločensky užitočné produkty alebo služby.

Oddelenie odmeny v meritoch od peňažnej odmeny vedie k virtuálnej rovnosti príjmov, keďže každý zarába rovnakú hodinovú mzdu v chronoch. Na druhej strane pracovníci NEO zostávajú motivovaní produkovať inovatívnu prácu, pretože hodnotné produkty — a ich tvorcovia — sú odmeňovaní nerovným množstvom meritov, ktoré odrážajú vytvorenú hodnotu.

**Základná štruktúra podávania výkazov**

- Každý pracovný výkaz vyplnený v Rováši patrí k **projektu** — entite, ktorá predstavuje vyrobený tovar alebo službu.
- Projekty môžu mať žiadny, jeden alebo viac pracovných výkazov. Ak projekt nemá žiadny pracovný výkaz, vlastník projektu nemôže byť odmenený chronmi. Stále však môže získať merity, ak je jeho projekt zakúpený alebo odmenený.
- Merity získané projektom si môžu rozdeliť **podielnici** projektu. Schému rozdelenia meritov stanovuje vlastník projektu alebo sa o nej dohodnú podielnici.
- Podielnikmi projektu môžu byť jednotlivci alebo iné projekty.
- Projekty môžu byť verejné alebo súkromné. V súkromných projektoch môže iba vlastník projektu pridávať alebo odoberať podielnikov a merity sa medzi nimi môžu rozdeliť podľa pevného podielu alebo podľa odpracovaného času. Verejné projekty sú otvorené pre každého a rozdelenie meritov je úmerné pracovnému času účastníka projektu.

**Čo je práca**

Práca v NEO má nasledujúce atribúty:

- je akákoľvek činnosť, ktorá bola preukázateľne vykonaná. Dôkazné bremeno je na používateľovi, ktorý prácu nahlasuje,
- výstup práce je dostupný zadarmo alebo sa dá zakúpiť za chrony,
- práca produkuje tovary alebo služby, ktoré nie sú spoločensky škodlivé.

**Podávanie pracovných výkazov**

Aby osoba mohla získať štandardných 10 chronov za každú odpracovanú hodinu, musí v Rováši vyplniť pracovný výkaz. Výkaz musí obsahovať nasledujúce informácie:

- dátum a čas začiatku práce (najviac 30 dní pred podaním pracovného výkazu),
- dĺžka odpracovaného času,
- hlavná kategória NACE, do ktorej činnosť patrí,
- dôkaz o tom, že nahlásená práca bola skutočne vykonaná. Poskytnutý dôkaz závisí od typu vykonanej práce; najčastejšie sa používa webová adresa a fotografie alebo videá zobrazujúce stav pred a po vykonaní práce.

Rováš má tiež API, ktoré umožňuje aplikáciám tretích strán odosielať pracovné výkazy v mene používateľov.

**Overovanie výkazov**

Po odoslaní pracovného výkazu používateľom Rováš vyberie dvoch ďalších používateľov a pošle im e-mailové upozornenie so žiadosťou o overenie výkazu. Overovatelia potom použijú dôkazy uvedené vo výkaze a ďalšie informácie poskytnuté Rovášom na schválenie alebo zamietnutie výkazu. Ak dvaja overovatelia vydajú odlišné rozhodnutia, Rováš určí a pozve ďalšieho — tretieho overovateľa. Po schválení Rováš okamžite odmení autora chronmi. Ak overovateľ neoverí výkaz do 5 dní, Rováš používateľa penalizuje a vyberie iného overovateľa.

**Dôveryhodní používatelia**

Používatelia, ktorých [skóre súladu](/glossary#complianceScore) dosiahne aspoň 50, alebo ktorí vlastnia projekt, z ktorého odmien získali viac ako 500 meritov, sa stávajú **[dôveryhodnými používateľmi](/glossary#trustedUser)**. Môžu si zvoliť okamžité schválenie svojich pracovných výkazov bez peer overovania a obmedziť alebo zastaviť prijímanie žiadostí o overenie. Tieto preferencie sa nastavujú v profile. Okamžite schválené výkazy je stále možné napadnúť sporom; prehra v spore môže status dôveryhodného používateľa odobrať, ak už nespĺňa podmienky.

**Tresty**

Ak používateľ overí žiadosť, k jeho verejne viditeľnému **skóre súladu** sa pripočíta jeden bod. Ak overovateľ na žiadosť o overenie nereaguje, je penalizovaný znížením skóre súladu o jeden bod. Jeden bod súladu sa tiež odpočíta od skóre používateľa, ak je zamietnutý pracovný výkaz, ktorý vyplnil. Keď skóre klesne na mínus tri body, Rováš odpočíta desať meritov od skóre meritov používateľa. Ak skóre meritov klesne pod nulu, jeho držiteľ stráca možnosť vytvárať pracovné výkazy a získavať tak chrony. Jeho schopnosť vytvárať projekty však zostáva zachovaná, čo mu umožňuje získavať merity, a preto opäť podávať výkazy a získavať chrony, keď jeho skóre meritov stúpne nad nulu.

Akékoľvek rozhodnutie o overení pracovného výkazu možno dodatočne napadnúť začatím formálneho konania s názvom **Spor**. V spore žalobca popíše dôvod sťažnosti a požiada obvineného o nápravu problému. Ak obvinený odmietne tak urobiť, obe strany zhromaždia podporovateľov svojho stanoviska. Po troch dňoch Rováš spor uzavrie a určí výsledok výpočtom kumulatívneho skóre meritov používateľov každej strany. Vyhráva strana s najväčším počtom meritov. Postup pri spore a sankcie sú podrobnejšie opísané na [stránke Spory](/disputes).

**Druhy odmeňovanej práce**

V NEO je akákoľvek aktivita odmenená, ak:

1. činnosť bola preukázateľne vykonaná. Toto posúdenie robia overovatelia, ktorí svoje rozhodnutie zakladajú najmä na dôkaze o práci poskytnutom používateľom podávajúcim pracovný výkaz,
2. práca bola vykonaná po registrácii používateľa v Rováši. Práca vykonaná predtým, ako si používateľ vytvoril účet v Rováši, nemôže byť odmenená,
3. práca začala najviac pred 30 dňami od podania pracovného výkazu. Prácu so starším dátumom začiatku nie je možné odmeniť,
4. práca produkuje tovary alebo služby, ktoré sú bezplatné alebo sa dajú kúpiť za chrony,
5. overovatelia sa domnievajú, že činnosť nie je v rozpore s hodnotami komunity.

**Transparentnosť**

Takmer všetky informácie v NEO sú voľne prístupné jeho používateľom. Výnimkou sú osobné kontaktné údaje (e-mailová adresa) a údaje o bankových účtoch v iných menách okrem chronu. To znamená, že každý používateľ si môže kedykoľvek skontrolovať pohyby na účte iného používateľa, ako aj sledovať, ako a za čo získal svoje chrony a merity. Toto pravidlo transparentnosti je potrebné na zabezpečenie stability ekonomiky NEO.

**Licencia na používanie údajov**

Akékoľvek údaje dostupné v Rováši sa môžu použiť iba na vytváranie produktov a služieb, pre ktoré existuje v Rováši projekt. Tvorcovia takýchto projektov musia pridať **[projekt Rováš](/node/1992)** do svojho projektu ako podielnika a prideliť mu podiel zodpovedajúci hodnote, ktorú údaje Rováš poskytujú projektu.
