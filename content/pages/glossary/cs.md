---
title: Glosář
format: filtered_html
---

**<a name="neo"></a>NEO**

NEO je ekonomický systém, kde může být jakákoli skutečně vykonaná činnost odměněna, pokud lze produkt této činnosti volně spotřebovat, nebo zaplacením měny NEO [chrony](#chron). Pracovníci NEO jsou odměňováni paušální paušální odměnou 10 chronů za každou hodinu své práce a pokud někdo zaplatí za produkt nebo službu vytvořenou touto prací, obdrží také odměnu [za Merit](#merit). Aby uživatelé získali chrony, musí podat [pracovní hlášení](#workReport) v aplikaci NEO [Rováš](#rovas), která vydá chronovou odměnu, jakmile hlášení [ověří](#verification) dva nebo více pracovníků NEO. Všechny transakce a pohyby a zůstatky na uživatelských účtech chrony a Meritů jsou veřejně viditelné, aby byla umožněna veřejná kontrola. Každý uživatel NEO může zahájit spor proti jinému uživateli, pokud má podezření, že porušil pravidla NEO.

**<a name="rovas"></a>Rováš**

Rováš je webová aplikace implementující pravidla NEO. Aplikaci se může zaregistrovat a používat kdokoli na její internetové adrese https://rovas.app nebo pomocí [Rováš API](https://neofund.sk/rovas-api).

**<a name="project"></a>Projekt**

Aby výrobce zboží nebo služby získal Merity, musí v Rováši vytvořit projekt. Kupující produktů nebo služeb pak mohou zaplatit za svou spotřebu kliknutím na tlačítko „Nákup/Odměna“ na stránce projektu. Vlastník projektu může určit, jak se Merity získané z prodeje produktu rozdělí mezi jednotlivé účastníky projektu nebo jiné účastníky projektu, kteří pomohli vytvořit dané zboží nebo službu, a to tak, že jim přidělí [podíly](#shareholder) na projektu. Podíly lze nastavit procentuálně nebo podle odpracované doby. Existují dva hlavní typy projektů – **soukromé**, kde vlastník projektu zve pracovníky a přiděluje jim podíly, nebo **veřejné**, do kterých se může zapojit kdokoli a kde jsou podíly přidělovány čistě podle odpracované doby nahlášené účastníky. Aby pracovník získal chronickou odměnu za práci na konkrétním projektu (a získal podíl na Meritech za svůj přínos), musí v daném projektu podat [pracovní zprávu](#workReport).

**<a name="systemProject"></a>Systémový projekt**

Po registraci vytvoří Rováš pro registrovaného uživatele speciální projekt, který mu usnadní získání odměny za vytvoření účtu Rováš. Tento projekt by neměl být používán pro jiné typy pracovních reportů.

**<a name="shareholder"></a>Akcionář**

Každý [projekt](#project) Rováš má jednoho nebo více akcionářů. Jsou to uživatelé, kteří mají právo zveřejňovat pracovní zprávy v projektu a dostávat podíl na Meritech udělených projektu, když někdo koupí nebo odmění produkt projektu. Podíl na Meritech, které akcionář obdrží, může být založen na délce pracovní doby investované do projektu, procentuální výši nebo kombinaci obou metod. Konkrétní alokace Meritů pro daný projekt určuje vlastníka projektu ve formuláři projektu.

**<a name="workReport"></a>Pracovní zpráva**

Pracovní zpráva je záznam o práci vykonané pracovníkem na konkrétním [projektu](#project). Pracovní zprávy obsahují datum zahájení práce, délku odpracované doby, kategorii NACE pro vykonanou práci a jeden nebo více typů dokladů, které prokazují, že práce byla skutečně provedena. [Pravidla NEO](/rules) stanoví, že odměnu za práci mohou získat pouze pracovní zprávy, které byly prokazatelně provedeny a byly podány po registraci pracovníka v Rováši. Je třeba poznamenat, že okamžitá užitečnost vykonané práce **není** podmínkou pro získání odměny za práci, což odráží skutečnost, že mnoho typů produktů nebo služeb získává společenskou hodnotu až po dlouhé době.

**<a name="verification"></a>Ověření**

Aby byla zajištěna stabilita ekonomiky NEO, musí být každá podaná pracovní zpráva schválena dvěma dalšími uživateli, aby se kvalifikovala na odměnu Chron. Výběr ověřovatelů provádí algoritmus Rováš, který vybírá nejvhodnější kandidáty podle jejich oblasti odbornosti, jak je uvedeno v kódech [NACE](#nace), a podle skóre uživatelů. Ověřovatelé jsou požádáni o ověření dané zprávy e-mailem a také viditelně umístěným oznámením v systému Rováš.

**<a name="chron"></a>Chron**

Chron je **měna** [ekonomického systému NEO](#neo). Kdokoli může získat chrony za práci, kterou rád dělá, a to podáním pracovní zprávy v [Rováše](#rovas). Poté, co dva uživatelé vybraní Rovášem usoudí, že nahlášený čas je realistický, Rováš udělí pracovníkovi, který zprávu vyplnil, 10 chrony za každou nahlášenou hodinu. Další podmínky nutné pro získání odměny chrony jsou uvedeny na [stránce Pravidla](/rules).

**<a name="merit"></a>Merit**

Merit je nepeněžní, nepřenosné, reputaci podobné **měřítko společenské hodnoty** práce, které se uděluje výrobcům zboží nebo služeb, když je někdo koupí nebo odmění jejich vlastními odměnami. Odměny se nevyplácejí výrobci zakoupeného zboží nebo služeb, ale společnosti Rováše, která je převádí na Merity - jeden chron za jeden Merit (zaplacené chrony se zničí). Meritové skóre uživatele je veličina odrážející mnohorozměrnost lidských schopností a dovedností, protože se skládá z Meritů za jednotlivé kategorie NACE, uvedených v jeho pracovních výkazech. Pokud se Meritové skóre uživatele stane záporným - například když jeho skóre shody klesne pod 0 - ztratí možnost zveřejňovat pracovní výkazy a získávat Merity. Může však i nadále vytvářet nové projekty a získávat Merity. Jakmile se jeho Meritové skóre se stane kladným, znovu získá přístup ke všem službám Rováš.

**<a name="complianceScore"></a>Skóre shody**

V NEO je každému uživateli přiřazeno číselné skóre, které odráží úroveň jeho souladu s pravidly NEO. Po vytvoření účtu v Rováš je skóre 0 a zvyšuje se o 1, když:

- uživatel [ověřuje](#verification) hlášení jiného uživatele

Skóre se sníží o 1, když:

- pracovní zpráva uživatele je odmítnuta
- uživatel neověří pracovní zprávu

Jakmile skóre shody uživatele dosáhne hodnoty -3, Rováš odečte 10 bodů od skóre uživatele a vynuluje skóre shody zpět na 0.

**<a name="trustedUser"></a>Důvěryhodný uživatel**

Důvěryhodný uživatel je účet v Rováši se stálým statusem důvěryhodnosti. K získání statusu stačí buď [skóre shody](#complianceScore) alespoň 50, nebo vlastnictví zveřejněného [projektu](#project), z jehož odměn uživatel získal více než 500 [Meritů](#merit) (samotné vlastnictví podílů nestačí). Po udělení status zůstává, dokud po prohře ve [sporu](#dispute) uživatel přestane splňovat podmínky. Důvěryhodný uživatel si může volitelně nechat vlastní [pracovní výkazy](#workReport) okamžitě schválit bez peer [ověření](#verification) a může omezit nebo zastavit přijímání žádostí o ověření. Tyto preference se nastavují v profilu. Jiní uživatelé mohou okamžitě schválené výkazy stále napadnout sporem.

**<a name="nace"></a>NACE**

Každý pracovní výkaz musí obsahovat informace o typu vykonané práce. NEO používá klasifikační schéma typů prací [NACE](https://en.wikipedia.org/wiki/Statistical_Classification_of_Economic_Activities_in_the_European_Community). Uživatel při podávání pracovního výkazu přiřadí práci správnou třídu NACE. Hlavním důvodem, proč musí výkazy tyto informace obsahovat, je usnadnění procesu ověřování. Rováš používá kód NACE k přiřazení daného pracovního výkazu k ověření uživatelům, kteří podali pracovní výkazy se stejnou nebo podobnou klasifikací NACE, aby se zajistila efektivita procesu ověřování. Přiřazení informací NACE k pracovním výkazům také usnadňuje nalezení pracovníků s požadovaným typem a úrovní odbornosti.

**<a name="dispute"></a>Spor**

Spor je postup, v němž může jeden uživatel NEO obvinit druhého z porušení pravidel NEO. Porušení se může týkat pracovního výkazu podaného za práci, která nebyla provedena, nebo takového, který generuje produkt či službu, kterou nelze volně konzumovat ani kupovat za chrony. Spomínaný dokument se identifikuje podle ID pracovního výkazu nebo ID projektu. Po podání sporu obě strany získávají do svých táborů [důvěryhodné uživatele](#trustedUser). Po uplynutí stanovené doby Rováš spočítá důvěryhodné uživatele v každém táboře; vyhrává tábor s větším počtem. Třetí spor, který pro obviněného neskončí výhrou ani remízou, je potrestán jako prohra, i když stížnost napraví. Podrobnější popis procesu naleznete na [stránce Spory](/disputes).

**<a id="projectMEalloc" name="projectMEalloc"></a>Rozdělení Meritů**

Vlastník [projektu](#project) může určit, jak budou odměny vyplacené projektu přiděleny. V poli Přidělení podle Meritů může buď ponechat prázdné, nebo nastavit číslo mezi 0 a 100. Číslo udává část částky vyplacené projektu (v procentech), která bude převedena na [Merity](#merit). Zbývající část platby je odeslána v [chronech](#chron) na účet vlastníka nebo příjemce projektu. Prázdná hodnota nastaví část odměny podle Meritů na 100 %.
