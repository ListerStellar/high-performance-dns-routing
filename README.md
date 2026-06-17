# **CDN77 Task: Сache/DNS**
- V adresari /task1 naleznete reseni prvni ulohy (wildcard DNS record) a reseni teoretickych otazek.
- V adresari /task2 naleznete reseni druhe ulohy (CDN DNS).

## Zpusob reseni a architektura 
- DNS wildcard (uloha 3): Ulohu jsem vyresil v C++. Pouzil jsem std::unordered_set. Vyhledavani tak probiha v prumernem case O(1) vzhledem k poctu ulozenych zaznamu. Algoritmus u prichozi domeny (po prevodu na lowercase) postupne odtrhava subdomeny zleva i zprava a testuje, zda v setu neexistuje odpovidajici wildcard zaznam.
- CDN DNS (uloha 2): Ulohu jsem vyresil v C++. Pro vyreseni jsem implementoval jednoduchy binarni strom. Algoritmus iteruje bit po bitu pres IPv6 adresu a zanoruje se do stromu. Behem pruchodu si pamatuje posledni platny pop_id a prefix_scope, cimz najde nejvic specificky subnet.
Teoreticke otazky Nginx: Analyzoval jsem dokumentaci Nginxu a castecne jeho kod.

## Zaseky a jejich reseni
- U CDN DNS jsem trochu zasekl na parsovani ipv6 adresy ze stringu do uint8_t, delat to rucne zni velmi neprijemne. Pak ale jsem dozvedel o funkci inet_pton, coz to krasne resi.

## Alternativni pristupy a skalovani
- Jak jsem si poznamenal v task2/node.md, muj jednoduchy binarni strom pro IPv6 sice funguje casove logaritmicky k delce IP adresy, ale ma spatnou prostorovou slozitost. U rozsahlych dat by to mohlo znamenat vytvoreni obrovskeho mnozstvi nodu v pameti. Mnohem efektivnejsim pristupem by bylo pouzit Radix Tree. Radix Tree by ale zabral mnohem vic casu na implementaci, proto jsem pro ucely tohoto zadani zvolil jednonuchy binarni strom.

## Co by slo zlepsit pro produkci
- Nahradil bych binarni strom za zmineny Radix Tree.
- V C++ kodu by bylo bezpecnejsi vymenit raw pointery a manualni spravu pameti za smart pointery, aby se predeslo memory leakum.
- Pridal bych mutexy, pokud by ke strukturam pristupovalo vice vlaken soucasne.
- Melo by pridat rozsahlejsi validace vstupnich dat, napr. osetreni spatne naformatovanych IPv6 adres v routing datech.
