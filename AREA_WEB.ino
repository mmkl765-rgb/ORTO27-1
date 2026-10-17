// FILE CONTENETE TUTTA LA PATRE DELLA GESTIONE WEB, seutp, loop e funzioni associate


  // FUNZIONE CHE AVVIA E SETTA LA PARTE WEB DEL SISTEMA: abilita la scheda, verifica la sua presenza e 
  // funzionalita'. In caso tutto ok, prima cerca di connettersi al router e se non lo trova (fuori
  // campo, spento, non funzionante) attiva AccessPoint per potersi collegare direttamente.
  // Idea per usarlo anche da telefonino come telecomando e visualizzatore generale del sistema.
  // Il setup finisce con l'avvio del server web " Server_Orto "
  // Collegandosi con gli IP fissi (messi sul file SECRET) si visualizza la pagina principale
void SETUP_WEB() { 
  // CONFIGURAZIONE CON IP STATICI 
  lcd.clear();
  strcpy_P(W_BUFF, PSTR(" - AVVIO AREA WEB - ")); SCRIVI_LCD(0, 0, W_BUFF);
  // se non c'e' la scheda o non funziona...
  if (WiFi.status() == WL_NO_MODULE) { 
    WifiOK = false;
    strcpy_P(W_BUFF, PSTR("ER-CHIP WIFI KO!    ")); SCRIVI_LCD(0, 1, W_BUFF);
    strcpy_P(W_BUFF, PSTR("AVVIO SENZA RETE... ")); SCRIVI_LCD(0, 2, W_BUFF);
    delay(1500); 
  } else { WifiOK = true; }
  // La prima volta cerca di collegarsi alla rete wifi del router (se presente) attingendo i valori definiti come "sta"
  // "sta" significa connessione modo station quindi ad un router che da accesso alla rete
  if (WifiOK) {
    // Tentativo connessione al router
    strcpy_P(W_BUFF, PSTR("CONNES. ROUTER..."));  SCRIVI_LCD(0, 1, W_BUFF);
    // APPLICO L'IP FISSO Sintassi: WiFi.config(IP, DNS, GATEWAY, SUBNET)
    WiFi.config(sta_IP, sta_dns, sta_gateway, sta_subnet);
    // Tentativo di connessione/test per 5 secondi (500 millis delay x 10 tentativi)
    WiFi.begin(ssid_router, pass_router);  // da impostare questi valori su file esterno tipo "SECRET.h" da includere
    int tentativi = 0;
    while (WiFi.status() != WL_CONNECTED && tentativi < 10) {
      delay(500);
      tentativi++;
    }
    // se connessione avvenuta con successo ...
    if (WiFi.status() == WL_CONNECTED) {
      strcpy_P(W_BUFF, PSTR("ROUTER CONNESSO"));  SCRIVI_LCD(0, 1, W_BUFF);
      snprintf(W_BUFF, sizeof(W_BUFF), "IP %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
      SCRIVI_LCD(0, 2, W_BUFF);
      WIFINET_AP = false;  // bool per sapere il tipo di connessione
      delay (1500);
    } else {
      // Se non si trova il router oppure non riesce a connettersi (credenziali errate o altro) esegue la seconda opzione creando
      // esso stesso un access point (AP) al quale ci si potrà collegare direttamente in locale tramite telefono, tablet, pc ...
      // Tutti i settaggi hanno la dicitura "AP" access point
      strcpy_P(W_BUFF, PSTR("AVVIO A.P. LOCALE")); SCRIVI_LCD(0, 1, W_BUFF);
      // WiFi.config(ap_IP);
      // genera la rete 192.168.4.1, cioè usando ip di default
      WiFi.beginAP(ssid_AP, pass_AP);
      IPAddress ipApCorrente = WiFi.softAPIP(); 
      snprintf(W_BUFF, sizeof(W_BUFF), "IP %d.%d.%d.%d", ipApCorrente[0], ipApCorrente[1], ipApCorrente[2], ipApCorrente[3]);
      SCRIVI_LCD(0, 2, W_BUFF);
      WIFINET_AP = true;  // bool per sapere il tipo di connessione
      strcpy_P(W_BUFF, PSTR("FINE WEB SETUP")); SCRIVI_LCD(0, 3, W_BUFF);
      delay(1500);
    }
    // Avvio server web, risponderà sia dall'IP del Router che dall'IP dell'AP
    Server_Orto.begin();
  }
// fine funzione Setup_Web
}

// FUNZIONE CHE GESTISCE IL LOOP DELLA PARTE WEB, funzionando con le variabili globali impostate
// nella parte alta del main.ino... La funzione controlla se c'e' un client collegato (tel, tablet, pc)
// e se risulta richiedere una connessione intercetta la richiesta  GET ed in base a quella carica la pagina
// corretta trasformando i tag tra gli intercettiri %  (es: %TAG%) nel relativo valore o dicitura da visualizzare
// tramite la funzione Converti_TAG(). Basta inserire la funzione dove si vuole mettere la parte web nel main.ino

// da sistemare i messaggi a lcd inserendo qualcosa del tipo "connessione web in atto" con scrivi_lcd(qualcosa da scrivere)

void LOOP_WEB() {
  if (WifiOK) {
    WiFiClient client = Server_Orto.available();
    if (client) {
      CLIENT_PRESENTE = true;              // modifica il bool, se c'e' una richiesta c'e' un client !
      ULTIMO_CONTATTO_CLIENT = millis();   // quando si è connesso =?=?=?=?=? 
      Serial.println("Telefono connesso! Elaborazione pagina... ");    
      // messaggio a LCD : avvertimento della connessione web e presenza client
      strcpy_P(W_BUFF, PSTR("CONNESSIONE WEB     ")); SCRIVI_LCD(0, 0, W_BUFF);
      strcpy_P(W_BUFF, PSTR("VERIFICA CLIENT = OK")); SCRIVI_LCD(0, 1, W_BUFF);
      strcpy_P(W_BUFF, PSTR("--SOLO COMADI WEB-- ")); SCRIVI_LCD(0, 2, W_BUFF);
      strcpy_P(W_BUFF, PSTR(" ELABORAZIONE...    ")); SCRIVI_LCD(0, 3, W_BUFF);
      
      String richiestaAttuale = ""; // Salverà solo la prima riga del comando
      String rigaCorrente = "";     // Per intercettare il doppio a capo \r\n\r\n

      // ALLOCAZIONE PREVENTIVA RAM per evitare la frammentazione...
      richiestaAttuale.reserve(100); // Sappiamo già che non supererà i 100 byte
      rigaCorrente.reserve(80);     // Una riga di header HTTP è raramente più lunga
    
      // --- TIMEOUT DI SICUREZZA: 2 secondi ---
      unsigned long tempoInizio = millis();
      const unsigned long timeout = 2000; 

      // Il ciclo gira finché il client è connesso E non è scaduto il timeout
      while (client.connected() && (millis() - tempoInizio < timeout)) {
        if (client.available()) {
          char c = client.read();

          // 1. Salviamo i primi 100 caratteri della richiesta (bastevoli per leggere l'URL)
          if (richiestaAttuale.length() < 100) { 
            richiestaAttuale += c; 
          }

          // 2. Intercettiamo la fine delle intestazioni HTTP (riga vuota \r\n)
          if (c == '\n') {
            // Se rigaCorrente è vuota, significa che abbiamo trovato due \n di seguito (\r\n\r\n)
            if (rigaCorrente.length() == 0) {
              
              // --- Caso 1. Intercettato il comando "bagna" => bagna tutto ---
              if (richiestaAttuale.indexOf("GET /bagna") != -1) {
                // inserire il codice macchina a stati per l'irrigazione forzata
                Serial.println("Attivazione: Irrigazione Totale (STATO 20)");
                Serial.println("Invio pagina di cortesia...");

                GESTISCI_PAGINA_WEB(client, PAGINA_IRRIGAZIONE);              
                break; // Esce dal ciclo while, la pagina è stata inviata
              }

              // --- Caso 2. Richiesta della Home Page ---
              else if (richiestaAttuale.indexOf("GET / ") != -1) { 
                Serial.println("Richiesta Home Page: Invio i dati dell'orto...");

                GESTISCI_PAGINA_WEB(client, HOME_PAGE);
                // METTERE IL COMANDO PER LA GESTIONE HOIME PAGE, CHE POI E' TUTTA AUTOMATICA
                // FORSE NON SERVE NULLA, SOLO LA GESTIONE PAGINA
                break; // Esce dal ciclo while
              }

              // --- Caso 3. Richiesta di pagine inesistenti o link errati (404) ---
              else {
                Serial.println("Richiesta sconosciuta! Errore 404");
                client.println("HTTP/1.1 404 Not Found"); 
                client.println("Content-Type: text/html; charset=utf-8"); 
                client.println("Connection: close");
                client.println(); 
                client.println("<html><body><div style='text-align:center;'><br><br><h2>ER 404 - Pagina non trovata nel sistema ORTO!</h2></div></body></html>");
                break; // Esce dal ciclo while
              }
            } else {
              // Nuova riga trovata, svuotiamo il buffer della riga per la successiva
              rigaCorrente = "";
            }
          } else if (c != '\r') {
            // Accumuliamo i caratteri della riga corrente (scartando il carattere \r)
            rigaCorrente += c;
          }
        } // Fine client.available()
      } // Fine ciclo while 

      // --- CHIUSURA CONNESSIOINE ---
      client.flush(); 
      delay(1); 
      client.stop();
      Serial.println("Connessione chiusa.");
      PULISCI_LCD();
    } // Fine controllo if(client)
    // Se non c'e' stata una richiesta del client da almeno 45 sec si puo considerare il client sconnesso
    if (CLIENT_PRESENTE && (millis() - ULTIMO_CONTATTO_CLIENT > 45000UL)) { CLIENT_PRESENTE = false; }
  } // Fine del if (WifiOK)
} // Fine della funzione LOOP_WEB

// -------------------------------------------------------------------------------------------------------------------------------------------------------
// GESTIONE PAGINE WEB - questa funzione gestisce l'invio delle pagine web in base alla pressione dei
// link intercettando  le pagine come comandi. inoltre trasforma i tag contenuti tra i "%" nei numeri o strighe 
// specifici in base al contesto tramite la funzione "processor". 
void GESTISCI_PAGINA_WEB(WiFiClient& client, const char* pagina) {
  // Invio intestazione "pagina esiste"
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println(); // Riga vuota obbligatoria

  // 1. Dichiariamo i due buffer vuoti e definiamo i "recinti" entro i quali devono stare i dati
  String bufferTag = "";
  String bufferInvio = ""; 
  // ricordarsi che nell'allocare la ram devi tener conto di letture al limite: sono arrivato a 199 caratteri (<200 per scrivere) e inizia il
  // tag che ne occupoa 10, finirei a 209 quindi con 9 in overflow !!! stai largo !!!! non scenere sotto il 230 meglio 250 !!!
  bufferInvio.reserve(250); // Alloca subito 250 byte in RAM per la dimensione del buffer dell'HTML
  bufferTag.reserve(30);    // Alloca subito 30 byte per contenere i nomi dei tag (es. %VERSIONE%)
  bool dentroIlTag = false;
  int i = 0;

  // --- 2. IL SETACCIO GENERICO IN TEMPO REALE ---
  // Leggiamo la pagina carattere per carattere finché non finisce ('\0')
  while (pagina[i] != '\0') {
    char carattereAttuale = pagina[i];
    if (carattereAttuale == '%') {          // se c'e' il carattere che => inizio tag
      if (!dentroIlTag) {                   // e non sei gia dentro il tag => % all'inizio
        // Inizio del tag: svuotiamo il buffer di testo normale inviando quello che c'è finora
        if (bufferInvio.length() > 0) {
          client.print(bufferInvio);   // invia quello che hai letto fin'ora..
          bufferInvio = "";            // poi svuotalo
        }
        dentroIlTag = true;
        bufferTag = "";
      }
      else {  // e come capisce che è alla fine del tag?????
        // Fine del tag: interpelliamo il doganiere (processor) e incolliamo il valore vero
        dentroIlTag = false;
        bufferInvio += Converti_TAG(bufferTag);
      }
    } else {
      if (dentroIlTag) {
        bufferTag += carattereAttuale; // Stiamo accumulando il nome del tag (es. VERSIONE)
      }
      else {
        bufferInvio += carattereAttuale; // Testo HTML normale
        // Per non finire la RAM dell'Arduino, ogni 200 caratteri svuotiamo il magazzino sul Wi-Fi
        if (bufferInvio.length() >= 200) {
          client.print(bufferInvio);
          bufferInvio = "";
        }
      }
    }
    i++;
  }

  // Svuotiamo l'ultimo carico rimasto nel buffer prima di chiudere la funzione
  if (bufferInvio.length() > 0) {
    client.print(bufferInvio);  // invia il resto se avanza qualosa...
  }
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------------------------------------------------
// --- FUNZIUONE PROCESSOR => CONVERTE I %TAG% IN VALORI O STRINGHE ---
/*    
      REFRESH_TIME
      VERSIONE
        STATO_CISTERNA 
        UMIDITA_Z1
        STATO_Z1
        UMIDITA_Z2
        STATO_Z2
        UPTIME
        MISURAZIONI
        IRRIGAZIONI
        da VAL14_Z1 a VAL0_Z1 (valori array storico Z1)
        da VAL14_Z2 a VAL0_Z2 (valori array storico Z2)
*/
String Converti_TAG(const String& var) {
  // VARIABILI DI APPOGGIO LOCALIZZATE da riutilizzare negli 'if' sottostanti
  unsigned long t = 0; uint8_t giorni = 0; uint8_t ore = 0; uint8_t minuti = 0;
  // GRAFICO SVG UNICO CON 2 LINEE, Zona 1 e Zona 2 con 2 colori
  if (var == "GRAFICO_SVG_WW") {
    String svg = "";
    svg.reserve(2350); // Buffer leggermente aumentato per contenere entrambe le linee, sperando bastino !!!
    // Contenitore SVG
    
    // DA QUI
    // copiare questa parte per le 3 zone modificando i colori per ogni zona (prendili dall'html)
    // limato gli spazi per rendere la String piu piccola possibile!
    svg += "<svg viewBox=\"0 0 540 100\" width=\"100%\" height=\"100\" style=\"overflow:visible;\">";
    // Griglia di sfondo (50%)
    svg += "<line x1=\"0\" y1=\"50\" x2=\"540\" y2=\"50\" stroke=\"#ccc\" stroke-dasharray=\"3\" />";
    // --- LINEA ZONA 1 (Blu: #2196f3) ---
    svg += "<polyline fill=\"none\" stroke=\"#2196f3\" stroke-width=\"2.5\" points=\"";
    for (int i = 59; i >= 0; i--) {
      int x = (59-i)*9;
      uint8_t valore = ZONA[1].STORICO_UMIDITA[i]; 
      int y = 100-valore; 
      svg += String(x) + "," + String(y) + " ";
    }
    svg += "\" />";
    // A QUI


    // --- LINEA ZONA 2 (Arancione/Rossa: #ff5722) --- #ffc107
    svg += "<polyline fill=\"none\" stroke=\"#ff5722\" stroke-width=\"2.5\" points=\"";
    for (int i = 59; i >= 0; i--) {
      int x = (59 - i) * 5;
      uint8_t valore = ZONA[2].STORICO_UMIDITA[i]; 
      int y = 100 - ((valore * 100) / 100); 
      svg += String(x) + "," + String(y) + " ";
    }
    // --- LINEA ZONA 3 (Ambra: #ffc107) --- 
    svg += "<polyline fill=\"none\" stroke=\"#ffc107\" stroke-width=\"2.5\" points=\"";
    for (int i = 59; i >= 0; i--) {
      int x = (59 - i) * 5;
      uint8_t valore = ZONA[3].STORICO_UMIDITA[i]; 
      int y = 100 - ((valore * 100) / 100); 
      svg += String(x) + "," + String(y) + " ";
    }
    // terminatori tag html
    svg += "\" />"; svg += "</svg>";
    return svg;
  }
  
  if (var == "VERSIONE_WW")       { return String(VERSIONE);  }         // definita
  if (var == "STATO_CISTERNA_WW") { if (ACQUA_OK) { return String ("LIVELLO OK"); } }
  if (var == "UMIDITA_Z1_WW")     { return String(TERRENO.ZONA1[0]); }  // definita
  if (var == "STATO_Z1_WW") { 
    if (TERRENO.ZONA1[0] >= SOGLIARH_Z1 + 10) { return String ("TERRENO OK"); }
    if (TERRENO.ZONA1[0] < SOGLIARH_Z1 + 10 && TERRENO.ZONA1[0] >= SOGLIARH_Z1 ) { return String ("TERRENO AL LIMITE"); }
    if (TERRENO.ZONA1[0] < SOGLIARH_Z1) { return String ("TERRENO SECCO !"); }
  }
  if (var == "UMIDITA_Z2_WW")     { return String(TERRENO.ZONA2[0]); }  // definita
  if (var == "STATO_Z2_WW")       { 
    if (TERRENO.ZONA2[0] >= SOGLIARH_Z1 + 10) { return String ("TERRENO OK"); }
    if (TERRENO.ZONA2[0] < SOGLIARH_Z1 + 10 && TERRENO.ZONA2[0] >= SOGLIARH_Z1 ) { return String ("TERRENO AL LIMITE"); }
    if (TERRENO.ZONA2[0] < SOGLIARH_Z1) { return String ("TERRENO SECCO !"); }
  }
  if (var == "UPTIME_WW") { 
    t = millis() / 1000UL; // t in secondi
     giorni = t / 86400UL; ore = (t % 86400UL) / 3600; minuti = (t % 3600) / 60; // da secondi a minuti-ore-giorni
    snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("%02dGG %02d:%02d"), giorni, ore, minuti);
    return String (W_BUFF);
  }
  if (var == "REBOOT_WW") {
    t = (AUTO_RESET - millis()) / 1000UL; // t in secondi
    giorni = t / 86400UL; ore = (t % 86400UL) / 3600; minuti = (t % 3600) / 60; // da secondi a minuti-ore-giorni
    snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("%02dGG %02d:%02d"), giorni, ore, minuti);
    return String (W_BUFF);
  }
  if (var == "MISURAZIONI_WW")    { return String(TERRENO.NUMERO_MISURAZIONI); }
  if (var == "IRRIGAZIONI_WW")    { return String(TERRENO.NUMERO_IRRIGAZIONI); }
  // Se non trova corrispondenze, cancella il tag tornando nulla ("")
  return "";
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------
