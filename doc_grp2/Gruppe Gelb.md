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

### Dienstag, 06.12.
**11:00 bis 13:30 - Jan**  
Das verarbeitete canny Bild `contours` wird nun in eine Vogelperspektive mit Hilfe von  `cv::cuda::warpPerspective()` verzerrt.
In Versuchen sieht das Resultat besser aus, wenn man das Warping auf das `contours` anstatt auf `src` anwendet. Dies muss aber nicht immer so sein... 
die ROI eingrenzung muss nun warscheinlich nicht mehr durchgeführt werden, da das Bild nun eh größtenteils nur die Fahrbahn abbildet. 
Da parallel Fahrstreifen nun auch im Bild parallel sind, wird in der Funktion `isEqual()` nun auch die Distanz berücksichtigt. 
Mit hilfe der `clusteredLines` wird nun mit Hilfe von `getAngle()` provisorisch ein erster Lenkwinkel auf der Konsole ausgegeben.
dieser ist jedoch noch anfällig für ausreißende Linien.
Eine Idee zur Lösung des Problems ist, die hough transformation anzupassen. Eine Vernachlässigung dieser Ausreißer könnte Probleme bei Kurven verursachen.
Ein Kommentar von Frauke: Vielleicht können wir einen Median-Filter verwenden, um die Ausreißer loszuwerden?


----

