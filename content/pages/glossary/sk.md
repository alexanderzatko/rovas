---
title: Glosár
format: filtered_html
---

**<a name="neo"></a>NEO**

NEO je ekonomický systém, ktorý odmeňuje každú skutočne vykonanú činnosť, pokiaľ táto nebola zaplatená za menu inú ako [chron](#chron) a ktorej produkt je bezplatný, alebo je k dispozícii za chrony. Pracovníci sú v NEO odmeňovaní paušálnou peňažnou odmenou 10 chronov za každú hodinu svojej práce a ak niekto zaplatí za produkt alebo službu vyrobenú touto prácou, sú pracovníci odmenení aj [meritmi](#merit). Ak chce používateľ získať chrony, musí podať pracovný výkaz v aplikácii [Rováš](#rovas). Akonáhle výkaz [overia](#verification) dvaja alebo viacerí iní používatelia NEO, Rováš vyplatí za výkaz chrony. Všetky transakcie a pohyby na chronových a meritových účtoch, používateľa sú verejne viditeľné, čo umožňuje verejnú kontrolu. Každý používateľ NEO môže začať proti inému používateľovi [spor](#dispute), ak má podozrenie, že porušil pravidlá NEO.

**<a name="rovas"></a>Rováš**

Rováš je webová aplikácia ekonomického systému NEO. Ktokoľvek sa môže zaregistrovať a používať aplikáciu priamo na je [internetovej adrese](https://rovas.app/), alebo prostredníctvom programovacieho rozhrania [Rováš API](https://neofund.sk/rovas-api).

**<a name="project"></a>Projekt**

Na získanie odmeny v [chronoch](#chron) alebo [meritoch](#merit), musí producent tovaru alebo služieb vytvoriť v Rováši projekt. Kupujúci môžu potom za tieto produkty alebo služby zaplatiť, kliknutím na tlačidlo na stránke projektu. Vlastník projektu musí určiť, ako budú rozdelené merity za platby medzi [podielnikov](#shareholder) projektu. Podiely je možné nastaviť percentuálne alebo podľa odpracovaného času. Existujú dva hlavné typy projektov - **súkromné**, kde vlastník projektu pozýva pracovníkov a prideľuje im podiely, alebo **verejné**, do ktorých sa môže ktokoľvek zapojiť. Vo verejných projektoch sú podiely prideľované výhradne podľa pracovného času nahláseného podielnikmi. Aby pracovník mohol získať odmenu v chronoch za prácu na konkrétnom projekte, musí v ňom mať podiel a podať v ňom [pracovný výkaz](#workReport).

**<a name="systemProject"></a>Systémový projekt**

Pri registrácii nového používateľa, Rováš preň vytvorí vytvorí špeciálny projekt, ktorý mu uľahčí získanie odmeny za vytvorenie účtu. Tento projekt by nemal byť používaný pre iné typy pracovných výkazov.

**<a name="shareholder"></a>Podielnik**

Každý [projekt](#project) v Rováš[i](#rovas) má jedného alebo viacerých podielnikov. Sú to používatelia, ktorí majú právo pridávať do projektu [pracovné výkazy](#workReport) a dostávať podiel v meritoch projektu ak niekto kúpi, alebo odmení produkt projektu. Merit podiel, ktorý podielnik dostane môže byť určený dĺžkou odpracovného času v projekte, percentom, alebo kombináciou týchto dvoch metód. Konkrétne rozdelenie podielu meritov pre daný projekt určuje jeho vlastník.

**<a name="workReport"></a>Pracovný výkaz**

Pracovný výkaz je záznam práce, vykonanej pracovníkom na konkrétnom [projekte](#project). Pracovné výkazy obsahujú dátum, kedy bola práca začatá, dĺžku pracovného času, kategóriu [NACE](#nace) za vykonanú prácu a jeden alebo viac druhov dôkazov, ktoré preukazujú, že práca bola skutočne vykonaná. [Pravidlá NEO](/rules) diktujú, že iba pracovné správy, ktoré boli preukázateľne vykonané a podané potom, čo sa pracovník zaregistroval v Rováši môžu byť odmenené [chronmi](#chron). Pri overovaní výkazov, **užitočnosť vykonanej práce nie je faktorom**, ktorý by bránil získaniu odmeny v chronoch, čo odráža skutočnosť, že mnoho druhov výrobkov alebo služieb získava spoločenskú hodnotu až po často veľmi dlhom čase.

**<a name="verification"></a>Overovanie**

Na zaistenie stability ekonomiky [NEO](#neo) musí byť každý podaný pracovný výkaz schválený dvoma ďalšími používateľmi, aby mohol byť odmenený [chronmi](#chron). Výber overovateľov sa vykonáva pomocou algoritmu v Rováš[i](#rovas), pričom kandidáti sú vyberaní podľa oblasti odbornosti použijúc kódy [NACE](#nace) z [pracovných výkazov](#workReport) a podľa ich [merit](#merit) skóre. Overovatelia sú vyzvaní na overenie e-mailovou správou, ako aj prostredníctvom viditeľne umiestneného oznamu v Rováši.

**<a name="chron"></a>Chron**

Chron je **menou** [ekonomického systému NEO](#neo), ktorú môže získať ktokoľvek za prácu, ktorú rád robí, podaním [pracovného výkazu](#workReport) v Rováši. Keď dvaja Rovášom vyzvaní [overovatelia](#verification) usúdia, že nahlásený čas je realistický, Rováš vyplatí pracovníkovi, ktorý vyplnil správu, 10 chronov za každú odpracovanú hodinu. Ďalšie podmienky potrebné na získanie chronov sú uvedené na [stránke pravidiel](/rules).

**<a name="merit"></a>Merit**

Merit je nepeňažná, neprenosná, reputácii podobná **miera spoločenskej hodnoty práce** jednotlivca. Merity sa udeľujú tvorcom [projektov](#project), ak ich niekto za vlastné [chrony](#chron) odmení, alebo kúpi. Chrony zaplatené kupujúcim nedostane výrobca, ale Rováš, ktorý ich premení na merity - jeden chron za jeden merit (zaplatené chrony sú pri tom zničené). Merit skóre používateľa je veličina odrážajúca mnohorozmernosť ľudských schopností a zručností, keďže sa skladá z odmien pre jednotlivé NACE kategórie, uvedené v jeho pracovných výkazoch. Ak sa merit skóre používateľa stane záporným - napríklad keď jeho [skóre súladu s pravidlami](#complianceScore) poklesne pod 0 - používateľ stratí schopnosť zverejňovať pracovné výkazy a zarábať chrony. Nové projekty však môže naďalej vytvárať, aby mohol získať merity. Akonáhle sa jeho merit skóre stane znovu kladným, je mu vrátená schopnosť naplno využívať všetky služby v Rováši.

**<a name="complianceScore"></a>Skóre súladu s pravidlami**

V NEO má každý užívateľ pridelené číselné skóre, ktoré odráža jeho schopnosť dodržiavať [pravidlá systému](/rules). Po vytvorení účtu v Rováši je skóre nastavené na 0 a zvyšuje sa o 1, ak:

- používateľ [overí](#verification) správu iného používateľa

Skóre je naopak znížené o 1, ak:

- pracovný výkaz používateľa je zamietnutý
- používateľ neoverí pracovný výkaz ak je na to vyzvaný

Akonáhle skóre súladu používateľa dosiahne hodnotu -3 Rováš mu odčíta 10 meritov a skóre vynuluje.

**<a name="trustedUser"></a>Dôveryhodný používateľ**

Dôveryhodný používateľ je účet v Rováši so stálym statusom dôveryhodnosti. Na získanie statusu stačí buď [skóre súladu](#complianceScore) aspoň 50, alebo vlastníctvo zverejneného [projektu](#project), z ktorého odmien používateľ získal viac ako 500 [meritov](#merit) (samotné vlastníctvo podielov nestačí). Po udelení status zostáva, kým po prehre v [spore](#dispute) používateľ prestane spĺňať podmienky. Dôveryhodný používateľ si môže voliteľne nechať vlastné [pracovné výkazy](#workReport) okamžite schváliť bez peer [overovania](#verification) a môže obmedziť alebo zastaviť prijímanie žiadostí o overenie. Tieto preferencie sa nastavujú v profile. Iní používatelia môžu okamžite schválené výkazy stále napadnúť sporom.

**<a name="nace"></a>NACE**

Každý pracovný výkaz musí obsahovať informácie o druhu vykonanej práce. NEO používa schému klasifikácie pracovných typov [NACE](https://en.wikipedia.org/wiki/Statistical_Classification_of_Economic_Activities_in_the_European_Community). NACE kategóriu volí používateľ pri vyplňovaní pracovného výkazu. Hlavným dôvodom, prečo musia správy obsahovať túto informáciu, je podpora [procesu overovania](#verification) výkazov. Rováš používa kód NACE na pridelenie daného výkazu na overenie takým používateľom, ktorí podali vlastné výkazy s rovnakou alebo podobnou kategóriou NACE, aby bola zabezpečená účinnosť procesu overovania. Priradenie kategórií NACE k pracovným výkazom tiež uľahčuje vyhľadávanie pracovníkov s požadovaným typom a úrovňou odbornosti.

**<a name="dispute"></a>Spor**

Spor je procedúra, pri ktorej jeden používateľ môže obviniť iného, z porušenia [pravidiel NEO](/rules). Porušenie sa môže týkať napríklad [pracovného výkazu](#workReport) za prácu, ktorá nebola nevykonaná, alebo takého, ktorého produkt nie je možné požívať bezplatne alebo získať za [chrony](#chron). Po podaní sporu si obe strany vyžiadajú podporu od ostatných používateľov, ktorí dajú svoj hlas svojmu kandidátovi. Po uplynutí stanovenej doby sa kumulatívne [merit](#merit) skóre priaznivcov oboch strán spočíta a obidve čísla sa porovnajú. Strana, ktorá má viac meritov vyhráva. Podrobnejší popis procesu je uvedený na stránke [Spory](/disputes).

**<a id="projectMEalloc" name="projectMEalloc"></a>Meritová časť odmeny**

Vlastník [projektu](#project) v ňom môže nastaviť, ako bude naložené s odmenami vyplatenými projektu. V projektovom formulári je na to pole **Meritová časť odmeny**, ktoré môže nechať prázdne, alebo doň vpísať číslo medzi 0 a 100. Číslo udáva tú časť sumy vyplatenej projektu (v percentách), ktorá bude premenená na [merity](#merit). Zvyšná časť platby je zaslaná v [chronoch](#chron) na účet vlastníka, alebo benefaktora projektu. Prázdna hodnota nastaví meritovú časť odmeny na 100%.
