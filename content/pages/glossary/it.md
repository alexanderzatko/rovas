---
title: Glosář
format: filtered_html
---

**<a name="neo"></a>NEO**

NEO è un sistema economico in cui qualsiasi attività effettivamente svolta può essere ricompensata, se il prodotto di tale attività può essere consumato liberamente, oppure pagando con la valuta NEO, [i chron](#chron). I lavoratori NEO vengono ricompensati con un compenso fisso di 10 chron per ogni ora di lavoro e, se qualcuno paga per il prodotto o servizio generato da tale lavoro, ricevono anche un premio [di merito](#merit). Per guadagnare chron, gli utenti devono compilare un [rapporto di lavoro](#workReport) nell'applicazione NEO [Rovas](#rovas), che emette un premio in chron una volta che il rapporto viene [verificato](#verification) da due o più lavoratori NEO. Tutte le transazioni e i movimenti e i saldi dei conti chron e di merito degli utenti sono visibili pubblicamente per consentire il controllo pubblico. Qualsiasi utente NEO può aprire una controversia contro un altro utente se sospetta che quest'ultimo abbia violato le regole NEO.

**<a name="rovas"></a>Rovas**

Rovas è un'applicazione web che implementa le regole NEO. Chiunque può registrarsi e utilizzare l'applicazione visitando il suo indirizzo internet all'indirizzo https://rovas.app o utilizzando l' [API di Rovas](https://neofund.sk/rovas-api).

**<a name="project"></a>Progetto**

Per guadagnare punti merito, il produttore di beni o servizi deve creare un progetto in Rovas. Gli acquirenti dei prodotti o servizi possono quindi pagare il loro consumo cliccando sul pulsante "Acquista/Ricompensa" nella pagina del progetto. Il proprietario del progetto può specificare come i punti merito guadagnati dalle vendite del prodotto vengono suddivisi tra i singoli partecipanti o gli altri partecipanti al progetto che hanno contribuito alla creazione del bene o del servizio prodotto, assegnando loro [delle quote](#shareholder) nel progetto. Le quote possono essere impostate in percentuale o in base al tempo di lavoro. Esistono due tipi principali di progetti: **privati**, in cui il proprietario del progetto invita i lavoratori e assegna loro le quote, o **pubblici**, a cui chiunque può partecipare e in cui le quote vengono assegnate esclusivamente in base al tempo di lavoro dichiarato dai partecipanti. Per guadagnare un punto merito per il lavoro svolto su un determinato progetto (e ottenere una quota di merito per il proprio contributo), il lavoratore deve presentare un [rapporto di lavoro](#workReport) in quel progetto.

**<a name="systemProject"></a>Progetto di sistema**

Al momento della registrazione, Rovas crea un progetto speciale per l'utente che si registra, al fine di facilitargli l'ottenimento di una ricompensa per la creazione del suo account Rovas. Questo progetto non deve essere utilizzato per altri tipi di report di lavoro.

**<a name="shareholder"></a>Azionista**

Ogni [progetto](#project) Rovas ha uno o più azionisti. Si tratta di utenti che hanno il diritto di pubblicare report di lavoro nel progetto e di ricevere una quota dei Meriti assegnati al progetto quando qualcuno acquista o premia il prodotto del progetto. La quota di Meriti che un azionista riceve può essere basata sulla durata del lavoro investito nel progetto, su una percentuale o su una combinazione di entrambi i metodi. La specifica assegnazione dei Meriti per un determinato progetto determina il proprietario del progetto nel modulo del progetto.

**<a name="workReport"></a>Rapporto di lavoro**

Il rapporto di lavoro è una registrazione del lavoro svolto da un lavoratore su un [progetto](#project) specifico. I rapporti di lavoro contengono la data di inizio del lavoro, la durata del lavoro, la categoria NACE del lavoro svolto e uno o più tipi di prove che attestano l'effettivo svolgimento del lavoro. Le [norme NEO](/rules) stabiliscono che solo i rapporti di lavoro che siano stati effettivamente eseguiti e che siano stati presentati dopo la registrazione del lavoratore in Rovas possono generare crediti. In particolare, l'utilità immediata del lavoro svolto **non** è una condizione per ottenere il riconoscimento dei crediti, a riprova del fatto che molti tipi di prodotti o servizi acquisiscono valore sociale solo dopo un lungo periodo di tempo.

**<a name="verification"></a>Verifica**

Per garantire la stabilità dell'economia NEO, ogni rapporto di lavoro presentato deve essere approvato da altri due utenti per poter ricevere la ricompensa in chron. La selezione dei verificatori avviene tramite un algoritmo Rovas, che sceglie i candidati più idonei in base al loro campo di competenza, come indicato dai codici [NACE](#nace), e ai punteggi di merito degli utenti. Ai verificatori viene richiesto di convalidare un determinato rapporto tramite e-mail e mediante un avviso ben visibile su Rovas.

**<a name="chron"></a>Crono**

Chron è la **valuta** del [sistema economico NEO](#neo). Chiunque può guadagnare chron svolgendo un lavoro che gli piace, compilando un rapporto di lavoro su [Rovas](#rovas). Dopo che i due utenti selezionati da Rovas avranno verificato la veridicità del tempo riportato, Rovas assegnerà al lavoratore che ha compilato il rapporto 10 chron per ogni ora di lavoro segnalata. Le altre condizioni necessarie per ottenere la ricompensa in chron sono elencate nella [pagina delle Regole](/rules).

**<a name="merit"></a>Merito**

Il merito è **una misura non monetaria, non trasferibile e simile alla reputazione, del valore sociale** del lavoro di una persona, conferita ai produttori di beni o servizi quando qualcuno li acquista o li ricompensa con i propri chron. I chron non vengono pagati al produttore del bene o servizio acquistato, ma a Rovas, che li converte in meriti: un chron per un merito (i chron pagati vengono distrutti). Il punteggio di merito dell'utente è una quantità che riflette la multidimensionalità delle capacità e delle competenze umane, in quanto è costituito da ricompense per le singole categorie NACE, elencate nei suoi report di lavoro. Se il punteggio di merito di un utente diventa negativo, ad esempio quando il suo punteggio di conformità scende al di sotto di 0, l'utente perde la possibilità di pubblicare report di lavoro e guadagnare chron. Tuttavia, può continuare a creare nuovi progetti e guadagnare meriti. Una volta che il suo punteggio di merito torna positivo, riacquista l'accesso a tutti i servizi di Rovas.

**<a name="complianceScore"></a>punteggio di conformità**

In NEO, a ogni utente viene assegnato un punteggio numerico che riflette il livello di conformità alle regole NEO. Dopo aver creato un account in Rovas, il punteggio è 0 e viene incrementato di 1 quando:

- l'utente [verifica](#verification) il report di un altro utente

Il punteggio viene diminuito di 1 quando:

- Il rapporto di lavoro dell'utente è stato rifiutato
- l'utente non riesce a verificare un rapporto di lavoro

Quando il punteggio di conformità dell'utente raggiunge il valore di -3, Rovas sottrae 10 punti merito dal punteggio dell'utente e lo azzera.

**<a name="nace"></a>NACE**

Ogni rapporto di lavoro deve contenere informazioni sul tipo di lavoro svolto. NEO utilizza lo schema di classificazione dei tipi di lavoro [NACE](https://en.wikipedia.org/wiki/Statistical_Classification_of_Economic_Activities_in_the_European_Community). La corretta classificazione NACE viene assegnata a un lavoro dall'utente al momento della compilazione del rapporto. Il motivo principale per cui i rapporti devono contenere queste informazioni è quello di agevolare il processo di verifica. Rovas utilizza il codice NACE per assegnare un determinato rapporto di lavoro, ai fini della verifica, agli utenti che hanno compilato rapporti di lavoro con la stessa classificazione NACE o con una classificazione simile, al fine di garantire l'efficienza del processo di verifica. L'assegnazione delle informazioni NACE ai rapporti di lavoro facilita inoltre la ricerca di lavoratori con il tipo e il livello di competenza desiderati.

**<a name="dispute"></a>Controversia**

La procedura di contestazione prevede che un utente NEO possa accusare un altro di aver violato le regole della piattaforma. La violazione potrebbe riguardare un report di lavoro non eseguito o la generazione di un prodotto o servizio impossibile da consumare o acquistare liberamente con i chron. Dopo aver presentato una contestazione, entrambe le parti sollecitano il supporto di altri utenti che votano per il proprio candidato. Trascorso un determinato periodo di tempo, i punteggi di merito cumulativi dei sostenitori di entrambe le parti vengono sommati e i due valori confrontati. La parte con il punteggio di merito più alto vince. Una descrizione più dettagliata del processo è disponibile nella [pagina dedicata alle contestazioni](/disputes).

**<a id="meritware" name="meritware"></a>Meritware**

Il proprietario di un progetto è libero di decidere come gestire le ricompense assegnate al suo progetto. L'impostazione predefinita per un progetto appena creato prevede la conversione del 100% di una ricompensa in meriti, rendendolo di fatto un **"meritware"**. Questo concetto serve a fornire un segnale chiaro: qualsiasi potenziale ricompensa al progetto verrà convertita al 100% in meriti e, pertanto, non arricchirà economicamente il proprietario del progetto.

**<a id="projectMEalloc" name="projectMEalloc"></a>Assegnazione dei premi al merito**

Il responsabile [di un progetto](#project) può specificare come verranno ripartiti i premi erogati al progetto. Utilizzando il campo "Allocazione dei premi" del modulo di progetto, può lasciarlo vuoto oppure inserire un numero compreso tra 0 e 100. Il numero indica la percentuale dell'importo erogato al progetto che verrà convertita in [premi](#merit). La parte restante del pagamento viene inviata in [rate,](#chron) sul conto del responsabile o al beneficiario del progetto. Un valore vuoto imposta la quota di premi in premi al 100%.
