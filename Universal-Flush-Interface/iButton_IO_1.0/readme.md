# iButtons + Binär Ein- und Ausgänge

Es werden bis zu 10 iButtons unterstützt, 2 Eingänge und 7 Ausgänge.
Achtung! Es stehen max ca. 25mA zur Verfügung! Bitte aufpassen wenn man LEDs an den Ausgängen nutzt!

Jeder iButton hat eigenes Kommunikationsobjekt(KO). 

Der Ausgang kann invertiert werden (*ab*wesend = 1, *an*wesend = 0 statt *ab*wesend = 0, *an*wesend = 1)

Gemeinsame KOs:
- alle anwesend
- mindestens einer abwesend
- alle abwesend (dabei wird ein Wert und/oder eine Szenennummer nach ablauf bestimmter Zeit gesendet)

ob ein iButton bei diesen Abfragen berücksichtig werden soll, hängt von der Suite-Einstellung "Berücksichtigen" ab.

Zusätzlich gibt es eine Möglichkeit nicht registrierte iButtons zu melden:
- KO mit dem Status "nicht registrierter iButton gefunden" (ja/nein)
- KO mit der ID des iButtons
 
KO mit der ID des iButtons kann man zum auslesen von neuen iButtons verwenden: 
im Gruppenmonitor steht ein Telegramm mit folgendem Inhalt: 01 00 AA BB CC DD EE FF 00 00 00 00 00 00,
wobei **01 00 AA BB CC DD EE FF** die Adresse des iButtons ist. Die folgende Nullen haben keine Bedeutung.
Alternativ kann man die ID direkt von dem iButton ablesen:

![iButton ID](/Universal-Flush-Interface/iButton_1.1/iButtonID.png)

In die Suite muss man die ID ohne Leerzeichen eingeben (beginned mit "01"): **0100AABBCCDDEEFF**
