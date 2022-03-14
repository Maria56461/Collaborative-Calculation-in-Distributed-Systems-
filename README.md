# Collaborative-Calculation-in-Distributed-Systems-
MPI distributed program in which more processes (workers) are grouped in a topology consisting of three clusters. They perform calculations and can only communicate with their coordinator.

## Maria-Alexandra Barbu, 335CA

## Tema 3- Calcule colaborative in sisteme distribuite
-------------------------------------------------------------------------------

Task 1- Stabilirea Topologiei
Fiecare dintre cei 3 MASTER-i isi citeste workerii din fisierul
corespunzator si ii stocheaza intr-un vector. Apoi fiecare MASTER isi trimite
propria topologie catre ceilalti 2 MASTER-i (la randul lui, primeste
topologiile de la MASTER-ii colegi). Odata ce un MASTER are topologia finala,
o afiseaza in terminal. Apoi, fiecare MASTER trimite workerilor sai rangul lui,
astfel toti workerii vor sti cine le este coordonatorul. MASTERUL trimite catre
workeri si intreaga topologie, iar acestia o afiseaza.  

Task 2- Realizarea calculelor
MASTER1 aloca si initializeaza un vector. Fiecare worker va prelucra
"N / nrWorkeri" elemente din vector, iar ultimul worker la lui MASTER3 va
prelucra in plus restul de elemente. MASTER1 trimite vectorul complet lui
MASTER2 si lui MASTER3. MASTER1 trimite workerilor sai bucata de vector pe care
trebuie sa o prelucreze, la fel MASTER2 si MASTER3. Fiecare worker primeste de
la coordonatorul sau sarcina sa de lucru si dubleaza elementele vectorului
primit (parte din vectorul initial). Apoi fiecare worker trimite noul vector la
coordonatorul sau. Coordonatorul strange toate bucatile de vector primite de la
workeri intr-un vector mai mare. MASTER2 si MASTER3 trimit fiecare lui MASTER1
vectorul format. MASTER1 uneste cei 3 vectori intr-unul mare si afiseaza
vectorul final obtinut. Cand trimit un vector, mereu trimit inainte si
dimensiunea acestiua . 
