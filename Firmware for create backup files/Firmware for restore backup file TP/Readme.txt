Tu sú 2 pomocné programy odvodené z kódu Atlan TPx, ktoré zapíšu súbor FNIRSI_1013D_tp_config.bin do rozsahu. To uľahčuje obnovenie súboru tp_config späť do rozsahu. V súbore FNIRSI_1013D_tp_config.bin napísanom programom fnirsi_1013d_fwb.bin nie sú potrebné žiadne zmeny a názov musí zostať, aby sa našiel správne. Tento nástroj zapíše 0x01 do posledného bajtu, aby signalizoval, že prichádzajú nové údaje, takže to nie je potrebné robiť s uloženým záložným súborom. Jediný bajt, ktorý možno bude potrebné zmeniť, je úplne prvý bajt v záložnom súbore, ak bolo do rozsahu po zálohe zapísané vyššie číslo, ako je v tomto súbore. Ak záložný súbor začína 0xFF, nebude problém. Súbor FNIRSI_1013D_tp_config.bin musí byť v adresári najvyššej úrovne na SD karte v rozsahu. Tento nástroj je zapísaný na kartu SD rovnako ako fnirsi_1013d_fwb.bin alebo ktorýkoľvek iný firmvér tu uverejnený pomocou dd alebo ekvivalentu.

Ak sa nenájde žiadny súbor FNIRSI_1013D_tp_config.bin, pomôcka ohlási chybu a nezapíše žiadne údaje do rozsahu.

Priložené sú 2 verzie. Prvým je: write_fnirsi_1013d_tp_config.bin.txt. Funguje to tak, ako je popísané vyššie. Druhý je: write_fnirsi_1013d_tp_config_save.bin.txt, ktorý funguje rovnako, ale navyše ukladá súbor s názvom my_tp_config.bin na SD kartu, čo sú tie isté údaje zapísané do rozsahu na overenie toho, čo bolo napísané.

Tieto fungujú s mojimi ďalekohľadmi, ale ako vždy ich používate na vlastné riziko!!

https://www.eevblog.com/forum/testgear/fnirsi-1013d-100mhz-tablet-oscilloscope/2050/

Here are 2 utilities derived from the Atlan TPx code that will write a FNIRSI_1013D_tp_config.bin file to the scope. This makes it easy to restore a tp_config file back to the scope. There are no changes needed to the FNIRSI_1013D_tp_config.bin file written by the fnirsi_1013d_fwb.bin program and the name has to remain to be found correctly. This utility will write an 0x01 in the last byte to signal that fresh data is coming so this does not have to be done to the saved back up file. The only byte that might have to be changed is the very first byte in the backup file if a number higher than what is in this file has been written to the scope subsequent to the backup.  If the backup file starts with 0xFF there won't be a problem. The FNIRSI_1013D_tp_config.bin file needs to be in the top level directory of the SD card in the scope. This utility itself is written to the SD card the same as fnirsi_1013d_fwb.bin or any of the other firmware posted here using dd or equivalent.

If there is no FNIRSI_1013D_tp_config.bin file found then the utility will report an error and will not write any data to the scope.

There are 2 versions attached. The first is: write_fnirsi_1013d_tp_config.bin.txt. This works as described above. The second is: write_fnirsi_1013d_tp_config_save.bin.txt which works the same but additionally stores a file named my_tp_config.bin on the SD card which is the same data written to the scope to verify what was written.

These work with my scopes but as always use at your own risk!!