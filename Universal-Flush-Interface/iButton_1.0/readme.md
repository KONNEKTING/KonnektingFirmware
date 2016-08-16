# iButtons

Es werden bis zu 10 iButtons unterstützt
Jeder iButton hat eigene Kommunikationsobjekte(KO). 
Der Ausgang kann invertiert werden (*ab*wesend = 1, *an*wesend = 0 statt *ab*wesend = 0, *an*wesend = 1)

Es gibt auch gemeinsame KOs:
- Alle anwesend
- mindestens einer abwesend
- alle abwesend

ob ein iButton bei diesen Abfragen berücksichtig werden soll, hängt von der Suite-Einstellung "Berücksichtigen" ab.

Zusätzlich gibt es eine Möglichkeit nicht registrierte iButtons zu melden
1. KO mit dem Status "nicht registrierter iButton gefunden" (ja/nein)
2. KO mit der ID des iButtons
 
KO mit ID des iButtons kann man zum auslesen von neuen iButtons verwenden: 
im Guruppenmonitor steht ein Telegramm mit folgendem Inhalt: 01 00 AA BB CC DD EE FF 00 00 00 00 00 00,
wobei **01 00 AA BB CC DD EE FF** die Adresse des iButtons ist. Die andere Nullen haben keine Bedeutung.
Alternativ kann man die ID direkt von dem iButton ablesen:

![Alternativer Text](/iButtonID.png)

In die Suite muss man die ID ohne Leerzeichen eingeben (beginned mit "01"): **0100AABBCCDDEEFF**