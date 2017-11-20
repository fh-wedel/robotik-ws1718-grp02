# Gruppe 2 - Gelb

Teilnehmer:

- Frauke Jörgens
- Jan Ottmüller
- Franz Wernicke
- Thorger Dittmann
- Felix Maaß (Gruppenleiter)


### Unsere Aufgabenstellung

- Motor- und Lenkregelung + Nothalt
- Fahrbahnerkennung (gerade und kurve)
- Einer Wand folgen

- gegebenfalls Kreuzungen erkennen und anhalten

----

## Tagebuch

### Mittwoch, 15.11.
**14 bis 17 Uhr - Alle:**  
Heute haben wir uns die Demos zu Lane- und Markerdetection angesehen.

Wir verwenden ab jetzt in unserer sample Lanedetection nur den Blau-Kanal, um das blaue klebeband gut zu erkennen. Dies hat Hermann uns gegeben, um die Fahrbahnmarkierung zu erstellen.

Wir haben ein paar Streifen auf einen Leitz-Ordner aufgeklebt und es damit getestet.
Wir würden aber gern in Zukunft von RGB auf HSV/HSB umsteigen, damit auch unterschiedliche blautöne erkannt werden können (verschiedene Lichteinwirkung).
Darum haben wir versucht, einmal die Roh-Videodaten mittels OpenCV zu exportieren. Leider haben wir uns gut 45 Minuten daran aufgehangen, dass wir den falschen Codec (HFUV) verwendet haben und es so nie zu einer Datei kam.
Schließlich benutzen wir nun Motion-JPG (MJPG), welches einwandfrei funktioniert.

Frauke möchte sich gern eine Testaufnahme mal mit diskretem OpenCV Code (ohne ADTF drumherum) ansehen.

Darüber hinaus hat Franz sich einmal die Konfiguration der Radsensoren bzw. der Motorsteuerung genauer angeschaut.

Wir sind in der Lage die Demo zum laufen zu bringen und es erscheinen einige Diagramme. Darunter unter anderem auch der zurückgelegte Fahrtweg. Leider hat die Ordinate keine Einheit und 4000m/s sind dann doch etwas viel ;-)

----


### Donnerstag, 16.11.
**17 bis 18 Uhr - Franz, Frauke, Jan, Felix:**  
Heute wollten wir für Franz einmal richtige Werte des Autos aufnehmen, dass heißt Spannungen, USS, Lenkwinkel, etc..., sodass er sie zuhause abspielen kann.

Leider war es uns nicht möglich, die Motoren zum laufen zu bringen. Wir haben versucht, die Fernbedienung neu zu koppeln. Dies verlief einwandfrei, **jedoch tat sich nichts**.

Alle Steckkontakte waren unseren Erachtens richtig und der Akku hatte auch 8,16 Volt. Die Sicherung war intakt. Dennoch wollte weder Servo noch Antrieb reagieren.

Felix hat Hermann darüber bereits informiert.

Während Franz und Felix diese ganzen Dinge prüften, haben Frauke und Jan die **Linienerkennung** über den HSV-Farbraum (an welchem Jan zuhause gearbeitet hatte) fertiggestellt. Wir haben die Linienerkennung jetzt also soweit fertig, dass wir das blaue Klebeband vom Hintergrund unter bekannten Lichtverhältnissen sehr gut **(Danke Jan 😇)** vom Rest trennen können. Mit unserem Ordner-Test waren die Ergebnisse einwandfrei!

Daraufhin haben wir (Franz, Frauke und Felix) uns daran gesetzt, dass das git repo zusätzlich die ADTF-Konfigurationen mit beinhaltet.

**Der erste Versuch** es per `.gitignore` plus whitelisting (um die ganzen anderen Ordner nicht angeben zu müssen) zu lösen, schlug fehl. Die `.gitignore` Datei war auch nach mehreren Versuchen noch nicht richtig konfiguriert. Es wurden lediglich die Top-Level Dateien der gewhitelisteten Ordner hinzugefügt.

**Der zweite Versuch**, die gewünschten Ordner auszulagern und per **symlink** wieder in für ADTF an der gewünschten Stelle einzubinden, ging zunächst super. Die Dateien wurden gefunden und man konnte das Projekt ausführen.
Leider hatte wir ab da jedoch Probleme mit den **makefiles**, da sie den Build-Ordner nicht mehr finden konnten. Da in Zukunft hiermit höchstwahrscheinlich noch mehr Probleme aufgetreten wären, haben wir diese "Lösung" beerdigt.

**Zuletzt** haben Frauke und Felix also doch nochmal die `.gitignore`s ausgepackt und viel trial-and-error angewendet. Dies führte uns dann irgendwann dazu, dass git beim hinzufügen in einer Endlosschleife zu stecken schien.  
Wir haben uns den .git Ordner mal genauer angesehen und nach Anomalien gesucht. Dieser war inzwischen 1,7GB groß. Sehr merkwürdig.  
Wir suchten aber weiterhin die Doku ab und sind so von einer gitignore Konfiguration zur Nächsten gestapft.

Wir haben den Ordner gelöscht und ein neues repo initialisiert. **Gleicher Fehler**: `git add` führt zu keiner Ausgabe.

Tatsächlich sind wir erst nach ca. eine Stunde suchen, der Dateigröße des repos auf den Grund gegangen. Es war das Problem, dass wir **3GB Zeitaufnahmen** gemacht hatten, und git diese auch zur Versionierung hinzufügen wollte. Dass dies etwas dauern kann, ist verständlich.

Letztendlich haben wir diese Dateien gelöscht und siehe da: Unsere `.gitignore` Datei war wohl doch nicht falsch. Die "Endlosschleife" war Vergangenheit.

Auf solch ein Problem fällt man wohl nur einmal rein ;-)

19:45 haben wir dann schließlich das Licht ausschalten können.



----


#### Freitag, 17.11.
**11:00 bis 11:35 Uhr - Hermann, Franz, Felix:**  
Heute wieder ans Auto gesetzt und uns auf Fehlersuche begeben. Diesmal war Hermann dabei. Wir haben ihm die Probleme erneut geschildert. Wir haben die Kontakte der Einspeisung, der Sicherung und die des Abnehmers geprüft.
Zwischen den Polen von Sicherung und Abnehmer war kein wirklicher Durchgang zu messen (60kOhm).

**12 bis 12:30 Uhr - Timm Bostelmann, Hermann, Franz, Frauke, Felix:**  
Nach dem Essen holten wir uns Timm, den Modellbauexperten hinzu. Wir haben den Akku direkt mit dem Motorsteuergerät verbunden und es leuchtete auf. **Bingo! Es lag nicht an einem frittierten Steuergerät.**

Insofern gingen wir davon aus, dass das PCB einen Schlag weg hatte, aber tatsächlich stellte sich nach einem **Anruf beim Hersteller**, als dieser die Teilenummer des Boards wissen wollte, **zufälliger Weise** heraus, dass der Arduino nicht richtig saß und Hermann ihn nur wieder richtig in den Sockel drücken musste, damit alles wieder lief.

**15:00 bis 15:30 Uhr - Franz, Frauke, Felix (+ Lukas und Hendrik):**  
Wir haben Lukas und Hendrik, zwei Interessierte, bei Vorträgen zu KI aufgeschnappt. Wir erklärten ihnen, wie das Modell in Zusammenspiel mit dem Fahrzeug funktioniert oder halt eben nicht funktinoiert ;-).
Außerdem haben wir unsere gwewünschte Aufgabenstellung ein Mal etwas konkreter formuliert.

**15:30 bis 17:45 Uhr - Franz, Felix (+ Lukas):**  

Letztendlich haben Franz und ich an der Motorsteuerung gebastelt. Wir fanden heraus, dass die `USSStructs` und `USSStructPlaus` keine Unterschiede aufweisen und auch nie im Code referenziert werden. **Das dann zu Plausibilätsprüfung ;-)**

Des Weiteren ist uns aufgefallen, dass die Werte die aus dem `Wheel Converter` kommen **exponentiell wachsen**, die Geschwindigkeit zur Raddrehzahl und die zurückgelegte Distanz bei konstanter Geschwindigkeit.

Wir haben den Durchmesser der Reifen neu bestimmt und die konstanten des Umfangs im Code abgeändert.

Felix konzentrierte sich nebenbei auf die Bedienung und **Kommunikation mit dem Auto via VNC**. Wir können einen der anderen Robotik-PCs verwenden und dort `TigerVNC` installieren. Für den Host (das Auto) können wir `x11vnc` verwenden.

Ein Test auf Felix' virtueller Maschiene war erfolgreich :).

----
