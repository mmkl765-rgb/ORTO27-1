// HOME PAGE del sistema ORTO WEB. versione 1: con solo un bottone link per l'irrigazione di tutte le zone, 
// come fatto nella parte senza web, in futuro potrei fare l'irrigazione forzata per zona....

const char HOME_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="15">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Orto 2026 - Controllo Remoto</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f0f2f5; margin: 0; padding: 15px; color: #333; }
        h1 { text-align: center; color: #2e7d32; font-size: 24px; margin-bottom: 20px; }
        .container { max-width: 600px; margin: 0 auto; }
        .card { background: white; padding: 15px; margin-bottom: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .card-title { font-size: 18px; font-weight: bold; margin-bottom: 10px; display: flex; align-items: center; gap: 8px; justify-content: center; }
        .data-row { display: flex; justify-content: space-between; padding: 6px 0; border-bottom: 1px solid #eee; }
        .data-row:last-child { border-bottom: none; }
        
        /* STILI UNIFICATI PER LE RIGHE DELLE ZONE / ACQUA */
        .zone-row { display: flex; justify-content: space-between; align-items: center; padding: 8px 0; border-bottom: 1px solid #eee; font-size: 14px; }
        .zone-row:last-child { border-bottom: none; }
        
        .zone-name { font-weight: bold; width: 28%; text-align: left; }
        .zone-data  { width: 36%; text-align: center; }
        .zone-status { width: 36%; text-align: left; }
        
        /* Specifico per la tabella acqua a 4 colonne */
        .water-col { width: 24%; text-align: center; }
        .water-col:first-child { text-align: left; }

        .stato-ok { background-color: #e8f5e9; border-left: 5px solid #4caf50; color: #2e7d32; }
        .stato-allarme { background-color: #ffebee; border-left: 5px solid #f44336; color: #c62828; }
        
        .legenda-z1 { color: #2196f3; } /* Blu per Zona 1 */
        .legenda-z2 { color: #ff5722; } /* Arancione per Zona 2 */
        .legenda-z3 { color: #9c27b0; } /* Viola per Zona 3 */

        /* STILE UNICO PER TUTTI I BOTTONI BLU */
        .btn-bagna { 
            display: block; 
            text-align: center; 
            background: linear-gradient(135deg, #2196f3 0%, #1976d2 100%); 
            color: white; 
            padding: 12px 20px; 
            margin-top: 15px; 
            border-radius: 6px; 
            text-decoration: none; 
            font-weight: bold; 
            box-sizing: border-box;
            box-shadow: 0 4px 6px rgba(33, 150, 243, 0.3);
            transition: all 0.2s ease-in-out;
            letter-spacing: 0.5px;
        }
        .btn-bagna:hover { 
            background: linear-gradient(135deg, #1976d2 100%, #1565c0 100%);
            box-shadow: 0 6px 12px rgba(33, 150, 243, 0.4);
            transform: translateY(-1px);
        }
        .btn-bagna:active {
            transform: translateY(1px);
            box-shadow: 0 2px 4px rgba(33, 150, 243, 0.4);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿 🌿</h1>
        <h1>ORTO 2027 - Ver 101026 - WIFI</h1>
        
        <!-- CARD GESTIONE ACQUA -->
        <div class="card">
            <div class="card-title">&#128167;&#128167;&nbsp;&nbsp;&nbsp;&nbsp;GESTIONE ACQUA&nbsp;&nbsp;&nbsp;&nbsp;&#128167;&#128167;</div>
            <div class="data-row"><span>Livello Acqua Cisterna:</span> <strong>Acqua OK</strong></div> <!-- mettere vaiabile -->
            
            <!-- Tabella Acqua per Zona -->
            <div class="zone-row" style="font-weight: bold; color: #666;">
                <span class="water-col">Zona</span>
                <span class="water-col">Ultima irr.</span>
                <span class="water-col">Litri tot.</span>
                <span class="water-col">N. Irr.</span>
            </div>
            <div class="zone-row">
                <span class="zone-name legenda-z1">zucchine</span>
                <span class="water-col">10 litri</span>
                <span class="water-col">123 litri</span>
                <span class="water-col">12</span>
            </div>
            <div class="zone-row">
                <span class="zone-name legenda-z2">pomodori</span>
                <span class="water-col">3 litri</span>
                <span class="water-col">67 litri</span>
                <span class="water-col">4</span>
            </div>
            <div class="zone-row">
                <span class="zone-name legenda-z3">verdura</span>
                <span class="water-col">3 litri</span>
                <span class="water-col">39 litri</span>
                <span class="water-col">6</span>
            </div>

          <!-- NOTA A SCOMPARSA 1 -->
            <details style="margin-top: 15px; background: #f8f9fa; padding: 8px 12px; border-radius: 6px; font-size: 13px; border: 1px solid #e9ecef;">
                <summary style="cursor: pointer; font-weight: bold; color: #2e7d32; outline: none; display: flex; align-items: center; gap: 6px;">
                    <span>💡</span> Info Acqua e Riavvio
                </summary>
                <p style="margin: 8px 0 0 0; color: #555; line-height: 1.4;">
                    Se appare la dicitura <strong>Acqua Finita</strong>, il sistema è entrato in un "blocco" nel quale la parte operativa (irrigazione) è
		    bloccata per evitare la rottura e la cavitazione della pompa. Se dovesse capitare che vada in blocco durante una irrigazione, il sistema si 
		    mette in sicurezza da solo bloccando la pompa e chiudendo le valvole. Per ristabilire la modalità ordinaria è richiesto la presenza di un 
		    operatore che riempia la cisterna e poi prema il pulsante di riavvio. Il sistema ripartirà dal punto dove si era fermato rileggendo prima
		    di tutto il livello dell'acqua in cisterna.
                </p>
            </details>

        </div>

        <!-- CARD STATO DELLE ZONE -->
        <div class="card">
            <div class="card-title">&#129362;&nbsp;&nbsp;&#127793;&nbsp;&nbsp;STATO DELLE ZONE&nbsp;&nbsp;&#127793;&nbsp;&nbsp;&#129362;</div>

            <!-- Zona 1 (Zucchine) -->
            <div class="zone-row">
                <span class="zone-name legenda-z1"> ■ Zucchine</span>
                <span class="zone-data">Umidità: <strong>21 /100</strong></span>
                <span class="zone-status">Stato: <strong>OK</strong></span>
            </div>

            <!-- Zona 2 -->
            <div class="zone-row">
                <span class="zone-name legenda-z2"> ■ Cudumaru</span>
                <span class="zone-data">Umidità: <strong>21 /100</strong></span>
                <span class="zone-status">Stato: <strong>LIMITE</strong></span>
            </div>

            <!-- Zona 3 -->
            <div class="zone-row">
                <span class="zone-name legenda-z3"> ■ Verdura</span>
                <span class="zone-data">Umidità: <strong>21 /100</strong></span>
                <span class="zone-status">Stato: <strong>SECCO</strong></span>
            </div>

            <br>
            <div class="card-title" style="font-size: 15px;">ANDAMENTO</div>

            <svg width="100%" height="100" viewBox="0 0 540 70" style="background: #fafafa; border-radius: 4px;">
                <line x1="0" y1="50" x2="540" y2="50" stroke="#888" stroke-dasharray="5" />
                <!-- Linea 1 (Blu) -->
                <polyline fill="none" stroke="#2196f3" stroke-width="2.5" points="0,60 9,58 18,55 27,50 36,48 45,45 54,46 
                63,42 72,40 81,38 90,35 99,36 108,38 117,40 126,42 135,45 144,44 153,41 162,39 171,38 180,35 189,32 198,30 
                207,28 216,25 225,24 234,26 243,28 252,30 261,33 270,35 279,38 288,40 297,42 306,45 315,48 324,50 333,52 342,55 
                351,58 360,60 369,62 378,65 387,63 396,60 405,58 414,55 423,52 432,50 441,48 450,45 459,42 468,40 477,38 486,35 
                495,32 504,30 513,28 522,25 531,24" />
                <!-- Linea 2 (Arancione) -->
                <polyline fill="none" stroke="#ff5722" stroke-width="2.5" points="0,35 9,38 18,40 27,42 36,45 45,48 54,52 63,55 
                72,52 81,48 90,45 99,42 108,40 117,38 126,35 135,32 144,30 153,28 162,26 171,25 180,28 189,32 198,35 207,38 216,42 
                225,45 234,48 243,52 252,55 261,58 270,60 279,58 288,55 297,52 306,48 315,45 324,42 333,40 342,38 351,35 360,32 
                369,30 378,28 387,26 396,25 405,28 414,32 423,35 432,38 441,42 450,45 459,48 468,52 477,55 486,58 495,60 504,58 
                513,55 522,52 531,50" />
                <!-- Linea 3 (Viola) -->
                <polyline fill="none" stroke="#9c27b0" stroke-width="2.5" points="0,35 9,33 18,30 27,28 36,26 45,28 54,30 63,33 
                72,35 81,38 90,42 99,45 108,48 117,46 126,43 135,40 144,37 153,35 162,32 171,30 180,28 189,30 198,33 207,36 216,40 
                225,43 234,46 243,48 252,45 261,42 270,38 279,35 288,32 297,30 306,28 315,30 324,33 333,36 342,38 351,40 360,42 
                369,40 378,38 387,35 396,32 405,30 414,28 423,30 432,32 441,35 450,38 459,40 468,42 477,45 486,48 495,46 504,43 
                513,40 522,38 531,36" />
            </svg>



<!-- NOTA A SCOMPARSA 2  -->
            <details style="margin-top: 15px; background: #f8f9fa; padding: 8px 12px; border-radius: 6px; font-size: 13px; border: 1px solid #e9ecef;">
                <summary style="cursor: pointer; font-weight: bold; color: #2e7d32; outline: none; display: flex; align-items: center; gap: 6px;">
			📈&nbsp;&nbsp;Info Storico Umidità
                </summary>
                <p style="margin: 8px 0 0 0; color: #555; line-height: 1.4;">
		 Lo storico prende i dati direttamente dalla memoria di Arduino e li trasforma in un grafico. I grafici sono divisi per colore ed ogni colore 
		 è associato ad una specifica zona. Il trattegigo indica il valore soglia tra secco e bagnato. Il grafico prende in considerazione le misurazioni
		 delle ultime 30 ore.
                </p>
            </details>



        </div>

        <!-- CARD IRRIGAZIONE FORZATA -->
        <div class="card">
            <div class="card-title">&#128167;&#128167;&nbsp;&nbsp;&nbsp;&nbsp;IRRIGAZIONE FORZATA&nbsp;&nbsp;&nbsp;&nbsp;&#128167;&#128167;</div>
            <a class="btn-bagna" href="/bagna">&#128167; Avvia Irrigazione Forzata</a>
            <small style="display: block; text-align: center; color: #666; margin-top: 12px;">N.B.: Il sistema sembrerà bloccato durante l'irrigazione.</small>
        </div>
        
        <!-- CARD INFO SISTEMA -->
        <div class="card">
            <div class="card-title">🌿🌿&nbsp;&nbsp; INFO ORTO 2026 &nbsp;&nbsp;🌿🌿</div>
            <div class="data-row"><span>Versione SW:</span><strong>%VERSIONE_WW%</strong></div>
            <div class="data-row"><span>Acceso da:</span><strong>%UPTIME_WW%</strong></div>
            <div class="data-row"><span>Auto Riavvio tra:</span><strong>%REBOOT_WW%</strong></div>
            <div class="data-row"><span>Misurazioni Effettuate:</span><strong>%MISURAZIONI_WW%</strong></div>
            <div class="data-row"><span>Irrigazioni Effettuate:</span><strong>%IRRIGAZIONI_WW%</strong></div>
        </div> 

        <div style="text-align: center; font-size: 13px; color: #888; margin-top: 20px; margin-bottom: 10px;">
            ORTO AUTOMATICO - VERSIONE %VERSIONE_WW%<br>
            &copy; 2026 MIK. Tutti i diritti riservati. <br>
            <span style="font-size: 11px; color: #aaa;">Proprietà intellettuale dell'Orto</span>
        </div>
    </div>
</body>
</html>
)rawliteral";