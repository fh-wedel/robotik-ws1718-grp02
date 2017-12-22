# Gruppe 2 - Gelb

Teilnehmer:

- Frauke Jörgens
- Jan Ottmüller
- Franz Wernicke
- Thorger Dittmann
- Felix Maaß (Gruppenleiter)


### Unsere Aufgabenstellung

- Motor- und Lenkregelung + Nothalt
- Fahrbahnerkennung (Gerade und Kurve)
- Kreuzungen erkennen und anhalten

----

## Tagebuch

### Mittwoch, 15.11.
**14 bis 17 Uhr - Alle:**  
Heute haben wir uns die Demos zu Lane- und Markerdetection angesehen.

Wir verwenden ab jetzt in unserer sample Lanedetection nur den Blau-Kanal, um das blaue Klebeband gut zu erkennen. Dies hat Hermann uns gegeben, um die Fahrbahnmarkierung zu erstellen.

Wir haben ein paar Streifen auf einen Leitz-Ordner aufgeklebt und es damit getestet.
Wir würden aber gern in Zukunft von RGB auf HSV/HSB umsteigen, damit auch unterschiedliche Blautöne erkannt werden können (verschiedene Lichteinwirkung).
Darum haben wir versucht, einmal die Roh-Videodaten mittels OpenCV zu exportieren. Leider haben wir uns gut 45 Minuten daran aufgehangen, dass wir den falschen Codec (HFUV) verwendet haben und es so nie zu einer Datei kam.
Schließlich benutzen wir nun Motion-JPG (MJPG), welches einwandfrei funktioniert.

Frauke möchte sich gern eine Testaufnahme mal mit diskretem OpenCV Code (ohne ADTF drumherum) ansehen.

Darüber hinaus hat Franz sich einmal die Konfiguration der Radsensoren bzw. der Motorsteuerung genauer angeschaut.

Wir sind in der Lage die Demo zum Laufen zu bringen und es erscheinen einige Diagramme. Darunter unter anderem auch der zurückgelegte Fahrtweg. Leider hat die Ordinate keine Einheit und 4000m/s sind dann doch etwas viel ;-)


----


### Donnerstag, 16.11.
**17 bis 18 Uhr - Franz, Frauke, Jan, Felix:**  
Heute wollten wir für Franz einmal richtige Werte des Autos aufnehmen, dass heißt Spannungen, USS, Lenkwinkel, etc..., sodass er sie zuhause abspielen kann.

Leider war es uns nicht möglich, die Motoren zum laufen zu bringen. Wir haben versucht, die Fernbedienung neu zu koppeln. Dies verlief einwandfrei, **jedoch tat sich nichts**.

Alle Steckkontakte waren unseren Erachtens richtig und der Akku hatte auch 8,16 Volt. Die Sicherung war intakt. Dennoch wollte weder Servo noch Antrieb reagieren.

Felix hat Hermann darüber bereits informiert.

Während Franz und Felix diese ganzen Dinge prüften, haben Frauke und Jan die **Linienerkennung** über den HSV-Farbraum (an welchem Jan zuhause gearbeitet hatte) fertiggestellt. Wir haben die Linienerkennung jetzt also soweit fertig, dass wir das blaue Klebeband vom Hintergrund unter bekannten Lichtverhältnissen sehr gut **(Danke Jan 😇)** vom Rest trennen können. Mit unserem Ordner-Test waren die Ergebnisse einwandfrei!

Daraufhin haben wir (Franz, Frauke und Felix) uns daran gesetzt, dass das git repo zusätzlich die ADTF-Konfigurationen mit beinhaltet.

**Der erste Versuch**, es per `.gitignore` plus Whitelisting (um die ganzen anderen Ordner nicht angeben zu müssen) zu lösen, schlug fehl. Die `.gitignore` Datei war auch nach mehreren Versuchen noch nicht richtig konfiguriert. Es wurden lediglich die Top-Level Dateien der gewhitelisteten Ordner hinzugefügt.

**Der zweite Versuch**, die gewünschten Ordner auszulagern und per **symlink** wieder in für ADTF an der gewünschten Stelle einzubinden, ging zunächst super. Die Dateien wurden gefunden und man konnte das Projekt ausführen.
Leider hatte wir ab da jedoch Probleme mit den **cmakefiles**, da sie den Build-Ordner nicht mehr finden konnten. Da in Zukunft hiermit höchstwahrscheinlich noch mehr Probleme aufgetreten wären, haben wir diese "Lösung" beerdigt.

**Zuletzt** haben Frauke und Felix also doch nochmal die `.gitignore`s ausgepackt und viel trial-and-error angewendet. Dies führte uns dann irgendwann dazu, dass git beim Hinzufügen in einer Endlosschleife zu stecken schien.  
Wir haben uns den .git Ordner mal genauer angesehen und nach Anomalien gesucht. Dieser war inzwischen 1,7GB groß. Sehr merkwürdig.  
Wir suchten aber weiterhin die Doku ab und sind so von einer gitignore Konfiguration zur Nächsten gestapft.

Wir haben den Ordner gelöscht und ein neues repo initialisiert. **Gleicher Fehler**: `git add` führt zu keiner Ausgabe.

Tatsächlich sind wir erst nach ca. eine Stunde suchen der Dateigröße des repos auf den Grund gegangen. Es war das Problem, dass wir **3GB Zeitaufnahmen** gemacht hatten, und git diese auch zur Versionierung hinzufügen wollte. Dass dies etwas dauern kann, ist verständlich.

Letztendlich haben wir diese Dateien gelöscht und siehe da: Unsere `.gitignore` Datei war wohl doch nicht falsch. Die "Endlosschleife" war Vergangenheit.

Auf solch ein Problem fällt man wohl nur einmal rein ;-)

19:45 haben wir dann schließlich das Licht ausschalten können.


----


### Freitag, 17.11.
**11:00 bis 11:35 Uhr - Hermann, Franz, Felix:**  
Heute wieder ans Auto gesetzt und uns auf Fehlersuche begeben. Diesmal war Hermann dabei. Wir haben ihm die Probleme erneut geschildert. Wir haben die Kontakte der Einspeisung, der Sicherung und die des Abnehmers geprüft.
Zwischen den Polen von Sicherung und Abnehmer war kein wirklicher Durchgang zu messen (60kOhm).

**12 bis 12:30 Uhr - Timm Bostelmann, Hermann, Franz, Frauke, Felix:**  
Nach dem Essen holten wir uns Timm, den Modellbauexperten hinzu. Wir haben den Akku direkt mit dem Motorsteuergerät verbunden und es leuchtete auf. **Bingo! Es lag nicht an einem frittierten Steuergerät.**

Insofern gingen wir davon aus, dass das PCB einen Schlag weg hatte, aber tatsächlich stellte sich nach einem **Anruf beim Hersteller**, als dieser die Teilenummer des Boards wissen wollte, **zufälligerweise** heraus, dass der Arduino nicht richtig saß und Hermann ihn nur wieder richtig in den Sockel drücken musste, damit alles wieder lief.

**15:00 bis 15:30 Uhr - Franz, Frauke, Felix (+ Lukas und Hendrik):**  
Wir haben Lukas und Hendrik, zwei Interessierte, bei Vorträgen zu KI aufgeschnappt. Wir erklärten ihnen, wie das Modell in Zusammenspiel mit dem Fahrzeug funktioniert oder halt eben nicht funktinoiert ;-).
Außerdem haben wir unsere gewünschte Aufgabenstellung ein Mal etwas konkreter formuliert.

**15:30 bis 17:45 Uhr - Franz, Felix (+ Lukas):**  

Letztendlich haben Franz und ich an der Motorsteuerung gebastelt. Wir fanden heraus, dass die `USSStructs` und `USSStructPlaus` keine Unterschiede aufweisen und auch nie im Code referenziert werden. **Das dann zu Plausibilätsprüfung ;-)**

Des Weiteren ist uns aufgefallen, dass die Wert,e die aus dem `Wheel Converter` kommen, **exponentiell wachsen**, die Geschwindigkeit zur Raddrehzahl und die zurückgelegte Distanz bei konstanter Geschwindigkeit.

Wir haben den Durchmesser der Reifen neu bestimmt und die Konstanten des Umfangs im Code abgeändert.

Felix konzentrierte sich nebenbei auf die Bedienung und **Kommunikation mit dem Auto via VNC**. Wir können einen der anderen Robotik-PCs verwenden und dort `TigerVNC` installieren. Für den Host (das Auto) können wir `x11vnc` verwenden.

Ein Test auf Felix' virtueller Maschiene war erfolgreich :).


----


### Montag, 20.11.
**14:00 bis 17:30 Uhr - Frauke, Franz, Felix, Thorger:**  
Codecs - OpenCV wurde nicht mit FFMPEG Support gebuilded, deswegen kein `HFYU`-Codec. In Zukunft verwenden wir also `MJPG`.

Franz hat uns Zugriff über SSH Keys zu Github verschafft. Dafür muss noch ein Whitelisting für SSH des Robolabs erfolgen. Temporär verwenden wir ein Handy als Hotspot oder klinken uns per LAN ins Mitarbeiter-Netzwerk ein.

Wir verwenden nun `TightVNC` als Client und `x11vnc` als Server.
Ein Problem ist noch, dass beim Ausstöpseln der Monitore der VNC Client auf einen Bildschirm limitiert wird.

Rückfahrkamera war kurzzeitig nicht verfügbar. **Reboot tut gut**.


**17:30 bis 18:00 Uhr - Frauke, Franz, Felix:**  
Wir sind am Abend noch ein wenig mit dem Auto per Fernbedienung rumgefahren. Dabei hatten wir über WLAN/VNC die Kontrolle über Aufnahmen, etc.


**18:00 bis 19:10 Uhr - Franz, Felix:**  
Als wir dies nun alles zum Laufen gebracht hatten, klebten einigen blaue Streifen auf den Boden. Diese repräsentieren nun unsere Straße. Die Fahrbahn ist nach Augenmaß **einen Meter** breit.

Dann haben wir ein Szenario **'Links Abbiegen'** aufgenommen.


----


### Dienstag, 21.11.
**09:30 bis 10:55 Uhr - Frauke, Franz, Felix:**  

- Depth-Image-Processing
- bitwise_not (schwarz ist nah, weiß ist fern)
- Median-Filter (not working with radius of 7 and 16bit images)

**14:00 bis 15:30 Uhr - Alle:**  

- Position der RealSense Kamera + Halter Design
- Gedanken über Zeitplanung
- Thorger's VM zum Laufen bringen
- Scope-Displays zeigen falsche Werte an: exponentielles Verhalten -> Konsole zeigt lineares Verhalten

**15:30 bis 17:15 Uhr - Jan:**

Jan hat ein Model für eine Halterung der RealSense Kamera in TinkerCAD gebaut, und mit kurzer Hilfe von Timm Bostelmann gedruckt. Leider ist die Passform etwas zu eng. Ein weiterer versuch, mit angepasstem Modell erfolgt warscheinlich morgen. 


----


### Mittwoch, 22.11.
**14:00 bis 15:30 Uhr - Jan, Franz, Felix:**  

Erneuter 3D-Druck mit angepasstem Model. Diesmal können wir erfreut von einem **'Snug-Fit'** sprechen. :-). Bei Bedarf kann man die RealSense Kamera nun hier anbringen und kann somit mehr von der Straße vor dem Auto sehen. 

Jan hat begonnen sich mit der **Hough-Transformation** zu beschäftigen und sie auf 'Edges' im Kamerabild angewendet.
Erste Ergebnisse sind bereits erstaunlich gut.

Franz hat seine SSD an den Robotik-PC angeschlossen und kann dort mit Hardwarebeschleunigung die Aufnahmen auch flüssig abspielen.
Leider meckert der Grafiktreiber etwas rum, was dazu führt, dass der Desktop nicht erweitert sondern nur gespiegelt werden kann. **#NichtSoGeil :-(**

Felix schaut sich die Doku einzelner Filter an und macht sich weitere Gedanken zum Lenkregler.

**15:30 bis 17:30 Uhr - Franz, Felix:**  

Franz schraubt an einem Filter, der einfach nur einen Float-Wert ausgibt.
Diese Aufgabe klingt total trivial, ist aber alles andere als leicht zu lösen: **Ein Hoch auf ADTF!**  
Die Dokumentationen von existieren Dateien ist zwar existent, leider entspricht sie jedoch nicht dem Code (Kopierpastete).  
**Merke:** *"Keine Doku ist immer noch besser als falsche Doku!"*

Felix hat sich währenddessen mit dem Lenkregler und dessen Outputs beschäftigt. Er stieß auf die selben Probleme.
Wir setzten uns also zunächst das Ziel, die Struktur der Filter überhaupt erst einmal zu verstehen.

**17:30 bis 19:10 Uhr - Felix:**  
Als Franz dann verzweifelt das Weite gesucht hatte, warf Felix einen Blick in Hermann's Doku-Ordner zur Software. Tatsächlich gab es einen Eintrag 'Schreib deinen ersten Filter'. Dieser bestand aber mehr oder weniger aus diesen Punkten:

1. Kopiere den Demo-Ordner
2. Bennenne ihn um
3. Benenne die enthaltenen Dateien um
4. Passe die CMake an
5. Viel Glück!

Insofern -> **Nicht sehr hilfreich :-(**  
Felix musste als noch etwas weiter an seinem Filter verzweifeln. Schließlich ist ihm aufgefallen, dass die Datenübertragung zwischen zwei Filtern sich einer Art komplizierter `Dictionaries` bedient.

Letztendlich ist es nur ein *"Nimm mal diese Daten (Void\*) und baller sie in den Eimer dort drüben!"*. Formal sieht das dann wie folgt aus:

1. Größe der Daten mittels `IMediaSerializer` berechnen.
2. Speicher für ein `IMediaSample` alloziieren.
3. Einen `Write-Lock` holen. Man bekommt einen `IMediaCoderExt`
 zurück.
4. Eine ID für den *'Eimer'*, in den die Daten geschrieben werden können, vom `IMediaCoderExt` besorgen.
5. Schreibe **diesen Wert** in **diesen Eimer**.
6. Setze die aktuelle Zeit in das `IMediaSample` ein, damit jeder weiß, wann das Paket zugeschnürt wurde.
7. Übertrage das `IMediaSample`.


----


### Donnerstag, 23.11.
**17:00 bis 18:30 Uhr - Felix:**  

**17:00 bis 19:15 - Franz und Frauke**

Da wir heute aufgrund des anstehenden Besuchs der Ministerin im Projektraum gearbeitet haben und wir unsere blaue Straße nicht mitgenommen haben, mussten wir rot/orange Riesen-Steckerleisten zum Testen zu Rate ziehen. Für die Erkennung der Farben mussten wir nur an den Parametern `hueLow` und `hueHigh` schrauben. Wir haben den Bereich zwischen 0 und 10° gewählt.

Wir haben uns mit der GPU-Implementierung der Hough-Transformation beschäftigt. Es stellte sich heraus, dass das gar nicht so schwierig ist, man muss nur die richtigen Header (gute Kandidaten sind `<opencv2/cudaarithm.hpp>`, `<opencv2/core/cuda.hpp>` und `<opencv2/cudaimgproc.hpp>`) kennen und in vielen Fällen nur die `cv::Mat`'s per `upload` auf die GPU laden. Ergebnis ist eine `cv::cuda::GpuMat`. Ist man fertig mit allen GPU-Operationen, muss die `cv::cuda::GpuMat` wieder heruntergeladen werden und auf eine `cv::Mat` geschrieben werden. Vor viele Funktionen muss man auch nur `cv::cuda::` + `Funktionsname` schreiben. Fies wird es, wenn das gerade nicht reicht, und sich die komplette Signatur ändert! So ist das zum Beispiel bei der Funktion `cv::HoughLines`. Die wird in der GPU-Implementierung durch einen Pointer auf eine abstrakte Klasse repräsentiert. Eine Variable dieses Typs sieht dann so aus: `cv::Ptr<cv::cuda::HoughLinesDetector> hough`. Auf diesen Pointer kann man dann beispielsweise die Funktion `detect` aufrufen. Beide Operationen zusammen haben eine ähnliche Signatur wie die CPU-Implementierung von `cv::HoughLines`. Einige Funktionen gibt es auch nicht mit GPU-Unterstützung, zum Beispiel `cv::line`.

Nachdem alle Kompilier- und Laufzeitfehler behoben wurden, funktionierte es auch! **Und wir haben einen deutlichen Unterschied zwischen CPU- und GPU-Implementierung gemerkt**, sodass die GPU-Variante -- je nach Auflösung des Videos -- nahezu in Echtzeit abläuft. Dazu können wir aber gerne noch genauere Messungen vornehmen. Leider haben wir die CPU-Variante nicht committed, sodass wir die erst einmal wieder rekonstruieren müssen :-(.

Jetzt steht nur noch die Frage im Raum, wie man damit arbeiten kann, wenn man keine nVidia-Grafikkarte, oder keinen stationären PC hat (wie Frauke). Implementiert man erst für die CPU, und muss man dann alles ändern? Oder implementiert man "blind" in der GPU-API und testet dann Live am Auto? Vielleicht haben wir uns damit doch selbst ins Bein geschossen...


----


### Freitag, 24.11.
**16:00 bis - Felix, Franz und Frauke:**

Wir haben Franz die GPU-Implementierung gezeigt, die Farbwerte wieder an Blau angepasst.

Unsere alten Werte waren:
```
hueHigh:    120
hueLow:     90
Saturation: 120
Value:      ??
```

Unsere neuen Werte sind:
```
hueHigh:    120
hueLow:     100
Saturation: 80
Value:      2
```
~~Unsere alten Werte haben wir wohl nie committed, deshalb sind sie verloren gegangen. Vielleicht weiß Jan sie noch ...~~ Frauke hat sie in einem Kommentar gefunden (bis auf `Value` -- siehe oben)!


----


### Montag, 27.11.
**10:00 bis 14:00 Uhr - Felix und Franz:**  
Fertigstellung des `FloatValueGenerator` und Fehlersuche für den `RadiusToAngleConverter`. Es lag letztendlich an einer nicht initialisierten Variablen :]

**14:00 bis 15:30 Uhr - Felix, Franz, Frauke und Jan:**  
Mapping von Angle auf ServoValue im `RadiusToAngleConverter` erstellt.
Frauke und Jan haben sich weiter mit der Fahrbahnerkennung beschäftigt. Es wurde ein Algorithmus zur Bündelung ähnlicher Linien erstellt, da für eine Fahrbahnmarkierung mehrere Linien erkannt werden. Hierzu wird `cv::partition()` und eine eigene Hilfsmethode `isEqual()` verwendet. 

**15:30 bis 16:45 Uhr - Frauke und Jan:**  
S.o.

**18:30 bis 19:00 Uhr - Felix:**  
Test des `RadiusToAngleConverter`.
Inbetriebnahme des Fahrzeugs war geplant. Jedoch hat die Hardware versagt. Erst wollte Ubuntu nich mehr booten (im Recovery-Mode gings :]), danach klappte VNC nicht mehr.

Insofern muss der Test auf morgen verschoben werden.


----


### Dienstag, 28.11.
**21:00 bis 22:30 Uhr - Felix:**  
Felix hat den Converter in zwei Teile aufgetrennt. Wir können nun sowohl von einem **Radius zu einem Winkel**, als auch von einem **Winkel zu einem Servo-Stellwert** konvertieren.  
Diese Auftrennung ist sinnvoll, da Jan und Frauke anstatt Radien lieber Winkel ausgeben möchten.


----


### Mittwoch, 29.11.
**13:00 bis 14:15 Uhr - Jan:**  
Die Linienerkennug mit der Hough transformation funktioniert nur unzureichend. 
Jan hat nun wieder die ADTF Demo Variante mit den Grünen Punkten aktiviert und die parameter im Video Playback angepasst. 
Die Linien werden hier gut erkannt. Als nächstes müssen die gefundenen Punkte zu Linien zusammengefasst werden. 

### Freitag, 01.12.
**12:45 bis 13:50 Uhr - Frauke:**  
Frauke hat sich noch einmal die Bildverarbeitung angeschaut, um zu schauen, ob es nicht doch ohne die grünen Punkte geht (siehe Jan's Eintrag oben). Bisher ist aber noch nicht wirklich etwas dabei herumgekommen.

----


### Montag, 04.12.
**09:00 bis 09:20 Uhr - Frauke, Felix:**  
Unser Stick wird nicht mehr vom System erkannt. Sowohl im BIOS/Boot-Menü als auch aus einem laufenden Linux ist dieser nicht mehr zu sehen. Anscheinend hat die **Hardware versagt**. Hermann ist informiert und kümmert sich um Ersatz.  
Übergangsweise dürfen wir nun seinen *blauen* Stick verwenden.


----


### Dienstag, 05.12.
**08:00 bis 09:00 - Frauke**  
Frauke hat sich weiter mit der Trennung von Bildverarbeitungsteil für den ADTF-Filter und die Bildverarbeitung in der LaneDetection gekümmert. Die beiden Dinge sind nun getrennt, OpenCV-Operationen wurden in die `bva._pp` ausgelagert.
Darüber hinaus hat sie im git-Repo den Ordner `config/hoe/` zu `config/grp2/` und die Projektdatei `config/hoe/hoe.prj` zu `config/grp2/grp2.prj` umbenannt, damit es nicht zu Konflikten oder Überschreibungen kommt, wenn wir auf Hermanns Stick arbeiten (git hat übrigens ganz schlau bemerkt, dass ich den Ordner und die Datei nur umbenannt habe :) ). Das Builden funktioniert weiterhin.
Beim Ausprobieren (Ausführen in ADTF) auf dem Auto fliegt die Exception: "CUDA driver version is insufficient for CUDA runtime version in function allocate". Anscheinend ist auf Hermanns Stick eine andere Version von CUDA installert, denn auf unserem Stick hatten wir keine Probleme mit der Version. Jetzt ist die Frage, wie wir damit umgehen ...

**09:00 bis 10:50 - Franz, Frauke, Felix, Hermann**  
Hermann hat den nVidia-Treiber wieder zum laufen gebracht. Was nach langem Suchen schlussendlich geholfen hat, war das `purge`n des nVidia-Treibers und die komplette Neuinstallation. Die BVA auf der GPU funktionierte dann wieder :)
Danach haben wir uns entschieden, Franz' SSD auf Hermann's alsten USB-Stick zu klonen. Das hat dank Hermanns Hilfe auch schnell funktioniert. Danke Hermann! :)
Danach haben wir aber einen git-Konflikt ausgelöst, der durch uncommittete Änderungen von Franz kam.

**12:30 bis 15:20 - Alle**
In der Mittagspause wurde der Stick an Fraukes Laptop angeschlossen, damit wir ihn dort booten können und das "kaputte" git wieder gerade zu biegen.
Oben in der Robotik gab es wieder den gleichen CUDA-Fehler wie vorher. Hermann hat das wieder geradegebogen. Danke Hermann x2.
Lenkwinkel neu vermessen, Hermann hillft bei NVidia Treibern, Aufnahme.

**15:20 bis 17:00 - Frauke, Jan**  
Es wurde weiter an der Linienerkennung und der Bündelung der Linien gearbeitet. Nachdem die Parameter verstanden und angepasst wurden, funktionierte dies auch recht gut. 


----

### Mittwoch, 06.12.
**11:00 bis 13:30 - Jan**  
Das verarbeitete canny Bild `contours` wird nun in eine Vogelperspektive mit Hilfe von  `cv::cuda::warpPerspective()` verzerrt.
In Versuchen sieht das Resultat besser aus, wenn man das Warping auf das `contours` anstatt auf `src` anwendet. Dies muss aber nicht immer so sein...

Die ROI Eingrenzung muss nun warscheinlich nicht mehr durchgeführt werden, da das Bild nun eh größtenteils nur die Fahrbahn abbildet.
Da parallele Fahrstreifen nun auch im Bild parallel sind, wird in der Funktion `isEqual()` nun auch die Distanz berücksichtigt. 
Aus den `clusteredLines` wird nun mit Hilfe von `getAngle()` provisorisch ein erster Lenkwinkel auf der Konsole ausgegeben.  
Dieser ist jedoch noch **anfällig für ausreißende Linien**.
Eine Idee zur Lösung des Problems ist, die Hough-Transformation anzupassen. Eine Vernachlässigung dieser Ausreißer könnte Probleme bei Kurven verursachen.

Ein Kommentar von Frauke: Vielleicht können wir **einen Median-Filter** verwenden, um die Ausreißer loszuwerden?


----

### Donnerstag, 07.12.
**10:30 - 12:00 Uhr - Felix, Franz**  
Zuerst haben wir das Repository wieder auf den Benutzer des Autos umgestellt. Jetzt funktioniert es wieder einwandfrei. 
Da noch einige commits auf dem Auto waren, wir aber schon zuhause weiter gemacht hatten, haben wir das lokale Repository auf den origin zurückgesetzt. Dazu sind die nicht hochgeladenen commits in den lokalen branch `bva-save` gesichert worden.

Danach haben wir noch den aktuellen Stand der bva getestet in dem wir das Auto auf die Straße gestellt haben. Um den Lenkwinkel anzuzeigen haben wir mit OpenCV den aktuellen Lenkwinkel in die Ausgabe gemalt.

Felix hat darüberhinaus die **Lineare Funktion** fertiggestellt. Sie hat 3 Inputs (`x`: Input, `g`: Gain, `o`: Offset) und einen Output (y).  
Der Output wird wie folgt berechnet: `y = gx + o`  
Sowohl Gain als auch Offset können durch Parameter in der Filterkonfiguration gesetzt und überschrieben werden.

Dieser Filter soll später eine Möglichkeit bieten, um die Geschwindigkeit innerhalb von Kurven herabzusetzen:
    
    Input = Geschwindigkeit
    Gain = Lenkwinkel
    Offset = 0

**17:00 - 19:00 Uhr - Felix**
Am Abend habe ich einen Median-Filter erstellt. Dieser führte allerdings noch zu einem **Segmentation Fault**.  
Der Filter besitzt eine variable Fenstergröße, die auch **live** während des Betriebes über die Filterparameter angepasst werden kann.


----


### Freitag, 08.12.
**14:20 - 16:00 Uhr - Felix, Franz**  
Gemeinsam haben wir die Ursache für den SegFault ausfindig machen können und ihn behoben. Letztendlich war es nur eine umgekehrte Subtraktion, was zu einem **Zugriff auf negative Array-Indizes** führte.  
Dies war leicht behoben und der Filter funktionierte hervorragend.

Wir setzen diesen Filter jetzt hinter der BVA für den Lenkwinkel ein. **Eine gute Fenstergröße scheint ~10 zu sein.**

Dann haben wir uns die Motorsteuerung genauer angesehen.
Den Code von Audi kann man echt in die Tonne treten. Er ist **durchzogen von Flüchtigkeitsfehlern** und hat unübersichtlich **viele Redundanzen**.

Der wohl schönste Fehler ist, dass wenn das Auto sich der genwünschten Geschwindigkeit annähert, die berechnete Regelabweichung dich vergrößert.
**Excuse me, Sir:** *What the \*\*\*\*?*

Es ist uns rätselhaft wie Audi da sagen kann, dass die Werte schon ganz gut passen und der Regler vernünftig funktionieren würde.

**16:00 - 19:00 Uhr - Frauke, Franz, Felix**  
Am Nachmittag haben wir Frauke die Auswirkung des Median-Filters gezeigt und weiter an der BVA rumgeschraubt.


----


### Montag, 11.12.
**14:20 - 15:20 Uhr - Felix, Franz, Frauke, Jan**  
Wir haben uns weitere Fortschritte der BVA angesehen und Franz' Regler weiter verbessert.

**15:20 - 18:20 Uhr - Frauke, Jan**  
Neues Konzept für die BVA. Es werden nun die **schwarz-weiß-schwarz Übergänge** markiert und aus den entstehenden Punkten die Linien erstellt. Wir erhoffen uns von dieser Methode, dass weniger 'FalsePositives' auftreten.

Grundsätzlich **klappt der Ansatz** schon ganz gut. Lediglich die **gestrichelten Leitlinien** werden noch etwas schlecht erkannt. Hier müssen die Parameter noch etwas getweakt werden.

Es gibt verschiedene Versionen, die wir ausprobiert haben:

**1)** Die Hough Line Transformation wird auf dem Binärbild angewandt, nachdem ein Canny-Filter angewandt wurde. Ergebnis sind 2 - 3 (manchmal sieht man die linke Linie nicht) gebündelte Linien wie in folgendem Bild:
```
/¦| bzw. / \
je nach Kameraposition und -ausrichtung
```
Das Problem hier ist, dass auch **viele Linien im Hintergrund** erkannt werden (z.B. Fenster/ Stühle). Dieses Problem soll mit Herangehensweise **1a)** und **1b)** gelöst werden. Ein weiteres Problem ist, dass die **gestrichelte Mittellinie meistens nur schwach** (= kleines Gewicht nach der Bündelung) oder gar nicht erkannt wird. Dieses Problem propagiert sich auf Varianten **1a)** und **1b)**.

**1a)** Wir wählen uns mit einer Trapezform eine binäre Maske, die wir auf das Binärbild der Straße anwenden. Das funktioniert sehr gut bei geraden Stücken. Bei Kurven hingegen wird auch ein Teil der Kurve **abgeschnitten**.

**1b)** Wir transformieren das mit mit `cv::warpPerspective` auf eine Art **"Vogelperspektive"**. Das Ergebnis sieht so ähnlich aus wie folgendes Bild:
```
|¦| bzw. ¦|
je nach Kameraposition und -ausrichtung
```
Vorteil: Der Lenkwinkel ist relativ leicht zu berechnen (ist dann aber auch etwas vereinfacht).
Nachteil: Wieder wird die Mittellinie nicht immer zuverlässig erkannt.

**2)** Wir nutzen die von Audi/ADTF vorgegebene LaneDetection, die in einem Interessenbereich (Region of Interest, ROI) Zeilenweise nach **Intensitätssprüngen** von Schwarz zu Weiß und zurück sucht und dort einen Punkt setzt, wo ein solcher Intensitätssprung erkannt wurde. Auf diese Punkte wenden wir wiederrum die Hough Line Transformation an und haben ein ähnliches Resultat wie in **1), nur mit weniger "Rauschen"** im Bild, sodass tatsächlich vor allem nur die Fahrbahnmarkierungen erkannt werden. Trotzdem ist die **Erkennung der Mittellinie weiterhin ein Problem**. Auch auf dieses Bild könnte man zur leichteren Winkelberechnung eine Perspektiven-Transformation wie in **1b)** beschrieben, anwenden.

Leider liefert keine der Varianten besonders gute Ergebnisse. Die Herangehensweise in **2)** liefert allerdings schon einmal ein viel sauberes Bild als in **1)**. Ein Problem, welches beiden Varianten gemein ist, ist, dass die Linien auf dem "untransformierten" Bild leider noch **zu flach erkannt** werden, weshalb u.A. extreme Lenkwinkel ausgegeben werden (**TODO -> Bild einfügen**). Außerdem ist in beiden Varianten die gestrichelte Mittellinie ein Problem.

**Eine Idee, die Frauke gerade beim Tippen kam:** Vielleicht können wir die Mittellinie auch durchgezogen lassen, damit wir zumindest testweise damit arbeiten können? Hermann hat gesagt, dass wir uns an einigen Stellen Sachen leichter machen können. Durchgezogene Mittellinien sind ja auch nicht ungewöhnlich.

Darüber hinaus haben wir die erkannten Linien nach der **Bündelung mit einem Gewicht** versehen, welches angibt, wie viele Linien zu einer Linie zusammengefasst wurden. Bei der Darstellung haben wir die **Linienbreite einer Linie entsprechend ihrer Gewichtung** (= Anzahl zusammengefasster Linien) berechnet, damit das ganze anschaulich wird.

Außerdem kam Hermann mit einem Päckchen vorbei, in dem sich die neue **Basler-Kamera** befand! Wir haben sie ausgepackt und sofort ausprobiert. Die Anbindung an ADTF hat super geklappt: Man muss nur den `BaslerCamera`-Block in die Konfiguration ziehen (z.B. indem man den `RealsenseCamera`-Block ersetzt) und nach Neustart der Konfiguration usw. auf Play drücken: Das Kamerabild wird ohne weitere Treiberinstallation o.Ä. angezeigt, da die Treiber schon vorinstalliert waren. Leider haben wir festgestellt, dass das **Sichtfeld der Kamera stark eingeschränkt ist** (ca. ~10°, entgegen der vermeintlichen 50 (?)°): Die mitgeliferte Linse hat eine Brennweite von 16mm. Hermann hat sich daran gemacht, weitere Objektive mit Brennweiten von 8 und 4 mm zu bestellen. Die Rückkamera hat eine Brennweite von 8mm. In der Zwischenzeit können wir uns Gedanken machen, **wo und wie wir die Basler-Kamera am Auto befestigen** können (vorne wie die Realsense? Schräg über dem Auto mit ca 45°-Blick auf die Straße? Über dem Auto, parallel zur Straße?). Außerdem stellt sich die Frage bei der Platzierung über dem Auto, ob und wie stark die Kamera dort **wackeln** könnte.

**18:20 - 19:15 Uhr - Felix, Franz**  
Wir haben kurz eine Sprungfunktion für die Motorregelung aufgenommen.
Gemessen haben wir Beschleunigungswerte von bis zu **0.5G beim Anfahren** und sogar bis zu **0.7G bei einer Vollbremsung**. Ganz schön beträchtlich.
Die Höchstgeschwindigkeit lag relativ konstant bei 4.8m/s.

Was uns aufgefallen ist: Die **Z-Beschleunigung fluktuiert stark**. Dies liegt wohl an dem nicht ganz ebenen Fliesenboden.
Hier wird Thorger gute Werte für seinen Accelerometer basierten Nothalt finden.

*Übrigens:* Donuts sind auch möglich. Höhö ;-)


----

### Dienstag, 12.12.
**09:00 - 10:50 Uhr - Felix, Franz, Frauke**  
Wir haben das Problem der zu flachen Linien in der BVA erkannt. Da könnte unseres Erachtens daran liegen, dass die Kamera **nicht richtig kalibriert** wurde. Also haben wir uns den ADTF-Filter `CameraCalibration` geschnappt und mit der Konfiguration für die Realsense-Kamera gefüttert. Leider stürzt ADTF nach dem Start aufgrund eines in dem `CameraCalibration`-Filter ausgelösten **OpenCV-Assertion-Errors** ab. In der Hoffnung, dieses Problem eventuell mit dem erneuten Komplieren der ADTF-Standard-Filter lösen zu können, haben wir das gemacht.

Wir haben das ADTF kaputt gemacht, indem wir `./build_all_eclipse` ausgeführt haben. Genauer gesagt wurden geraume Komponenten als `not available` und in Rot angezeigt. Wie sich später herausgestellt hat, **muss man die ADTF-Standard-Filter mit GCC Version 4.x kompilieren**, und nicht wie wir es getan haben, mit GCC Version 5.x. Es stellte sich auch wieder die Frage, warum der Wechsel zwischen den Versionen nochmal notwendig war ...?

**12:30 bis 15:20 - Felix, Franz, Frauke und Jan**  
Wir haben uns von Hermann zu unseren Problemen in der BVA beraten lassen. Er sagt: **"Macht es euch an einigen Stellen leichter"**. Wir möchten also nun vielleicht im Bild die erkannten Linien/Punkte als **linke und rechte Fahrbahnmarkierung** erkennen und mithilfe dieses Wissens (hoffentlich) bessere Winkel ausgeben. Darüber hinaus möchte Felix nun auch in der BVA mithelfen, da er mit seinen sonstigen Aufgaben fertig ist.

Danach hat sich Felix mit der **genaueren perspektivischen Transformation** beschäftigt, indem er die **Kalibrierung mit einem Schachbrettmuster** vornimmt. Dazu haben wir auch das RGB-Bild der Kamera perspektivisch transformiert. Wir sind allerdings nicht fertig gewordern; Felix wollte sich das am Abend noch einmal anschauen.

Weil die Ausgabe von debug-Videostreams momentan ziemlich mühselig ist, hatte Frauke folgende Idee: Man könnte auf verschiedenen Stufen die entsprechenden Bilder als `outputStream` ausgeben, sodass man sie sich im ADTF anzeigen kann. Alternativ könnte man einzelne **OpenCV-Funktionen auch als eigenen ADTF-Filter** implementieren (z.B. `cv::warpPerspective()`, `cv::threshhold` oder `cv::houghLines()` usw.), sodass wir eine Modularität schaffen. Da können natürlich viele Filter entstehen. Vielleicht kann man auch mehrere Operationen in einem Filter zusammenfassen.

----

### Mittwoch, 13.12.
**14:00 - 15:50 Uhr - Jan**  
Jan hat den Winkel der selbstgebauten RealSense Kamerahalterung erhöht, sodass diese nun besser auf die Straße ausgerichtet ist. 
Dies hat zu Folge, dass das transformierte Bild nicht mehr so anfällig bei der Beschleunigung ist. 
Ebenfalls hat Jan danach die perspektivische Transformation angepasst. Hierfür wurden zur Zeit ungenutzte Parameter im ADTF benutzt, um nicht ständig neu compilieren zu müssen. 
Nachdem die neuen Werte gefunden wurden, wurden sie in `findLines()` übertragen. Die Sichtweite nach vorne wurde deutlich verringert, da wir nicht vorrausschauend fahren müssen, sondern uns nur für den **aktuellen** Lenkwinkel interessieren


----


### Freitag, 15.12.
**07:50 bis 09:30 - Felix**  
Felix begann Hilfsfunktionen zur Klassifizierung der erkannten Linien zu schreiben. In Zukunft soll es möglich sein, zwischen rechten, linken und Haltelinien zu unterscheiden.


----


### Montag, 18.12.
**14:00 - 18:30 Uhr - Franz, Felix, Frauke, Jan**  
Wir haben uns um die Klassifizierung von Linien gekümmert und diese nun in unterschiedlichen Farben auf dem `Canny-Bild` ausgegeben.

Die Klassifizierung wird auf die bereits geclusterten Linien angewendet.
Es wird zwischen linken, rechten und Haltelinien unterschieden.

Zusätzlich hat die BVA jetzt einen Geschwindigkeitsausgang erhalten, womit es uns nun möglich ist, an Haltelinien anzuhalten (**Notiz: 40% der Linien müssen Haltelinien sein**).

**20:50 bis 23:00 Uhr - Felix (, Frauke)**  
Felix hat sich der Spurhaltung zugewandt. Es entstanden mehrere Hilfsfunktionen, die auf den erkannten Linien arbeiten.
Es sollte möglich sein, den x-Wert einer Linie auf Basis eines zugehörigen y-Wert (und umgekehrt) zu erlangen.

Die hierzu benötigten *Winkelberechnungen* wurden mit Frauke diskutiert und verifiziert.

----


### Dienstag, 19.12.
**09:00 - 10:45 Uhr - Franz, Felix, Frauke**  
Frauke und Felix haben sich zuerst mit der Einrichtung von **Teletype™️** auseinandergesetzt. Die Verbindung über das FH-Robotik-Netzwerk zwischen Felix' und Fraukes Laptop funktioniert gut -- man kann also gleichzeitig an einer File schreiben --, allerdings schafft es Teletype™️ nicht, auf dem Auto eine Verbindung aufzubauen. Es könnte daran liegen, dass Teletype™️ versucht einen Port zu öffnen, der nicht freigeschaltet ist ...

Danach haben sie sich weiter mit der BVA und der Berechnung von Schnittpunkten von Linien mit x- und y-Werten auseinandergesetzt. Man kann also für eine Linie einen bestimmten x- oder y-Wert angeben, und bekommt den jeweils anderen Wert für die Linie zurück.

**14:00 - 15:20 Uhr - Alle**  
Wir beheben den SegFault in Franz' Nothalt-Pin und kümmern uns um weitere Anpassungen des Spurhalte*assistenten*.  
Wir bereiten Alles darauf vor, dass das Auto sich selbst **auf der Spur zentrieren** kann.
Darüberhinaus sind wir nun soweit, dass das Auto eigenständig anhält, wenn es sich sicher ist, eine Haltelinie erkannt zu haben.

**15:20 - 17:30 Uhr - Frauke, Jan**  
Da das Anhalten recht früh und abrupt erfolgt, sind nun Vorkehrungen getroffen worden, die die gewünschte Geschwindigkeit mithilfe der aktuellen Position der Haltelinie im Kamerabild modulieren.
So ist ein **sanftes Abbremsen** gewährleistet.

**15:30 - 17:00 Uhr - Franz, Frauke**
Wir haben das Zentrieren auf der Fahrspur getestet und eine neue Vorangehensweise überlegt. Dann haben ein paar Videos aufgenommen und die VNC Verbindung auf Franz Surface ausprobiert, was sehr gut funktionierte.

Anmerkung (Felix): Zur Zeit wird dies durch eine Lineare Funktion bewerkstelligt. In Zukunft könnte hier die **Sigmoid-Funktion** (S-förmig) angewendet werden. Dadurch erlangen wir einen sanften Start gefolgt von einer *"Beschleunigung"* durch die Mitte, hin zu einem sanften Ausklingen.
