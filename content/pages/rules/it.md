---
title: Pravidla NEO
format: filtered_html
---

**Introduzione**

*Consultare il **[Glossario](/glossary)** se il significato dei termini qui utilizzati non è chiaro.*

**Rovas** è un'applicazione del sistema economico **NEO**, in cui i partecipanti vengono ricompensati per qualsiasi attività scelgano o inventino. NEO prevede due tipi di ricompense. Una è monetaria, assegnata per l'impegno lavorativo, sotto forma di valuta **chron**. Viene assegnata quando un report di lavoro inviato da un utente NEO viene approvato da altri due utenti. L'importo erogato è proporzionale al tempo lavorato e tutti i tipi di lavoro sono ricompensati con la stessa tariffa oraria. I chron possono essere utilizzati per pagare beni o servizi prodotti in NEO, oppure scambiati con valute nazionali.

Gli utenti di NEO possono anche guadagnare un **premio di merito** non monetario e non trasferibile, che rappresenta una misura del valore del prodotto del lavoro. Gli acquirenti di prodotti e servizi non pagano al produttore, ma a Rovas, che converte i chron pagati in meriti, un merito per un chron. I chron pagati vengono successivamente distrutti e i nuovi meriti vengono assegnati al venditore. Il punteggio di merito riflette quindi la capacità di un individuo di creare prodotti o servizi socialmente utili.

Separare il riconoscimento del merito dal compenso economico si traduce in una virtuale uguaglianza di reddito, poiché tutti guadagnano la stessa paga oraria in chrons. D'altro canto, i lavoratori NEO rimangono motivati a produrre lavori innovativi, perché i prodotti di valore - e i loro creatori - vengono ricompensati con quantità di merito diverse che riflettono il valore prodotto.

**La struttura di reporting di base**

- Ogni rapporto di lavoro compilato in Rovas appartiene a un **progetto**, ovvero un'entità che rappresenta il bene o il servizio prodotto.
- I progetti possono avere zero, uno o più rapporti di lavoro. Se un progetto non ha rapporti di lavoro, il proprietario del progetto non può essere ricompensato con cron. Tuttavia, può comunque ricevere meriti, se il suo progetto viene acquistato o ricompensato,
- i meriti acquisiti da un progetto possono essere condivisi dagli **azionisti** di un progetto. Lo schema di allocazione dei meriti è stabilito dal proprietario del progetto o negoziato tra gli azionisti,
- Gli azionisti di un progetto possono essere individui o altri progetti,
- I progetti possono essere pubblici o privati. Nei progetti privati, solo il proprietario del progetto può aggiungere o rimuovere soci, e i meriti possono essere ripartiti tra di loro secondo una quota fissa o in base al tempo lavorato. I progetti pubblici sono aperti a tutti e l'assegnazione dei meriti è proporzionale al tempo di lavoro del partecipante al progetto.

**Che cos'è il lavoro?**

Lavorare in NEO presenta le seguenti caratteristiche:

- è qualsiasi attività che sia stata dimostrabilmente eseguita. L'onere della prova spetta all'utente che segnala il lavoro,
- il risultato del lavoro è disponibile gratuitamente, oppure può essere acquistato per chrons,
- il lavoro produce beni o servizi che non sono socialmente dannosi

**Lavoro di reportistica**

Per ricevere i 10 chron standard per ogni ora lavorata, è necessario compilare un rapporto di lavoro in Rovas. Il rapporto deve contenere le seguenti informazioni:

- la data e l'ora di inizio dei lavori,
- la durata del lavoro,
- la principale categoria NACE in cui rientra l'attività,
- Una prova che il lavoro segnalato sia stato effettivamente eseguito. La prova fornita dipende dal tipo di lavoro svolto; la più comune è un indirizzo web e foto o video che mostrino lo stato di avanzamento prima e dopo l'esecuzione del lavoro.

Rovas dispone anche di un'API che consente ad applicazioni di terze parti di inviare report di lavoro per conto degli utenti.

**Verifica dei rapporti**

Dopo che un utente ha inviato un report di lavoro, Rovas seleziona altri due utenti e invia loro una notifica via email con la richiesta di verificare il report. I verificatori utilizzano quindi le prove incluse nel report e altre informazioni fornite da Rovas per approvarlo o rifiutarlo. Se i due verificatori emettono verdetti diversi, Rovas designa e invita un terzo verificatore. Una volta approvato, Rovas ricompensa immediatamente l'autore con dei chron. Se un utente verificatore non convalida un report entro 5 giorni, Rovas penalizza l'utente e seleziona un altro verificatore.

**Sanzioni**

Se un utente verifica una richiesta, viene aggiunto un punto al suo **punteggio di conformità** pubblico. Se un verificatore non risponde a una richiesta di verifica, viene penalizzato con una riduzione di un punto del suo punteggio di conformità. Un punto di conformità viene inoltre sottratto dal punteggio di un utente se il rapporto di lavoro compilato viene rifiutato. Quando il punteggio scende a meno tre punti, Rovas sottrae dieci meriti dal punteggio di merito dell'utente. Se il punteggio di merito scende sotto zero, il suo titolare perde la possibilità di creare rapporti di lavoro e quindi di guadagnare chron. La sua capacità di creare progetti rimane tuttavia invariata, consentendogli di guadagnare meriti e quindi di presentare nuovamente rapporti e guadagnare chron, una volta che il suo punteggio di merito risale sopra zero.

Qualsiasi decisione di verifica del rapporto di lavoro può essere ulteriormente contestata aprendo una procedura formale denominata **"Contestazione"**. In una contestazione, il denunciante descrive il motivo del reclamo e chiede all'accusato di porre rimedio al problema. Se l'accusato si rifiuta di farlo, entrambe le parti accumulano prove a sostegno della propria posizione. Dopo tre giorni, Rovas chiuderà la contestazione e determinerà l'esito calcolando il punteggio di merito cumulativo degli utenti di ciascuna parte. La parte con il punteggio di merito più alto vince. La procedura di contestazione e le relative sanzioni sono descritte in modo più dettagliato nella [pagina "Contestazioni"](/disputes).

**Tipologie di lavoro premiate**

In NEO, ogni attività viene premiata se:

1. l'attività è stata dimostrabilmente eseguita. Tale giudizio è espresso dai verificatori, che basano la loro decisione principalmente sulla prova del lavoro fornita dall'utente che presenta il rapporto di lavoro,
2. Il lavoro è stato svolto dopo che l'utente si è registrato su Rovas. Il lavoro svolto prima che l'utente abbia creato un account su Rovas non può essere retribuito.
3. l'opera produce beni o servizi che sono gratuiti o possono essere acquistati con chrons,
4. I verificatori ritengono che l'attività non violi i valori della comunità.

**Trasparenza**

Quasi tutte le informazioni in NEO sono liberamente accessibili ai suoi utenti. Fanno eccezione i dati di contatto personali (indirizzo email) e le coordinate bancarie in valute diverse da chron. Ciò significa che ogni utente può in qualsiasi momento verificare i movimenti del conto di un altro utente, nonché tracciare come e per cosa ha acquisito i propri chron e meriti. Questa regola di trasparenza è necessaria per garantire la stabilità dell'economia di NEO.

**Licenza d'uso dei dati**

Tutti i dati disponibili in Rovas possono essere utilizzati esclusivamente per la creazione di prodotti e servizi per i quali esiste un progetto in Rovas. I creatori di tali progetti devono aggiungere il **[progetto Rovas](/node/1992)** al proprio progetto come azionista e assegnargli una quota corrispondente al valore dei dati che Rovas fornisce al progetto.
