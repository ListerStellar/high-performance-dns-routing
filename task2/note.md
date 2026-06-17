Tohle reseni by mohlo mit hroznou prostorovou slozitost az na 2ˆ129 nodu na rozsahlych routing datech...
Takze by bylo hodne udelat ne proste jednoduchy binarni strom, ale nejakou pokricilejsi strukturu jako treba Radix Tree. 
Tedy napr. aby misto tohoto stromu:

    1_1_1_0_1_0_0_1_1
                 \_0_1

jsme mely takovy

    1110100_1_1
           \_0_1

Takze misto 11 nodu v pameti bychom mely 5.
A tim bychom usetrily hodne pameti.

Radix Tree by ale zabralo mnohem vic casu na implementaci, proto jsem se rozhodl udelat proste bin. strom.