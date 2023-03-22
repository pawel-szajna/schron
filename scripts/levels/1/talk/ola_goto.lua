dialogue_start()

if not met_ola then
    text('Dziewczynka śledziła mnie wzrokiem od kiedy wszedłem do pomieszczenia, w którym ' ..
         'się znajduje. Nie jest zdziwiona ani zawstydzona tym, że jakiś obcy dorosły chce ' ..
         'z nią rozmawiać. Wiem, że dzieci często nie krępują się obecnością starszych, ale ' ..
         'w przypadku tej konkretnej dziewczynki nawet to sprawia, że czuję gęsią skórkę.')
end
met_ola = true
name = 'Dziewczynka'
if ola_name then
    name = 'Ola'
end
speech(name, 'Dzień dobry panu.')

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::start::

local options = {}
if not ola_name then
    table.insert(options, { 'imie', 'Jak masz na imię?' })
end
table.insert(options, { 'strasznie', 'Ten pokój wygląda strasznie, nie boisz się?' })
table.insert(options, { 'rodzice', 'Co się tu w ogóle stało i gdzie są twoi rodzice?' })
if not petard_taken then
    table.insert(options, { 'plecy', 'Co tam chowasz za plecami?' })
end
choice(options)

if result == 'imie' then
    goto imi
elseif result == 'strasznie' then
    goto stra
elseif result == 'rodzice' then
    goto rodzic
elseif result == 'plecy' then
    goto plecy
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::imi::

speech('Ola', 'Mam na imię Ola.')
ola_name = true
name = 'Ola'
goto start

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::stra::

speech(name, 'A czego miałabym się bać? Tutaj wszystko jest jak w domu, czerwono, ciepło...')
choice({ { 'dom', 'Twój dom tak wyglądał?' },
         { 'rozum', 'Co ty opowiadasz?! Jak w domu?! Dziecko, czyś ty rozum postradała?' },
         { 'rodzic', 'Co się tu w ogóle stało i gdzie są twoi rodzice?'} })

if result == 'dom' then
    goto dom
elseif result == 'rozum' then
    goto rozum
elseif result == 'rodzic' then
    goto rodzic
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::dom::

text('Dziewczynka uśmiecha się. Nie wygląda to tak, jak powinien wyglądać uśmiech dziecka, ' ..
     'bardziej przypomina minę jakiegoś małego, rannego i bardzo chorego zwierzątka. A jednak ' ..
     'jest w tym coś potwornego, co każe mi odruchowo się cofnąć. Jakby za fasadą niewinnej ' ..
     'twarzy skrzywdzonego dziecka kryła się jakaś głęboka, być może wypływająca z samego źródła ' ..
     'jej osobowości, nienawiść.')
speech(name, 'Tam było cieplej. Bardziej czerwono. Mokro. Ciągle coś robiło „bum”. Strasznie ' ..
             'lubiłam tego słuchać.')
text('Niech mnie szlag, jeśli wiem o co w tym chodzi...')
goto finish

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::rozum::

text('Dziewczynka otwiera szeroko oczy i chichocze pod nosem. Ten dźwięk wbija mi się w mózg, ' ..
     'jakby był szklanymi odłamkami.')
speech(name, 'Niech się pan tak nie denerwuje...')
text('Z jej twarzy nagle znika wyraz rozbawienia. Oczy przestają rzucać dookoła nieco zlęknione, ' ..
     'ale też typowo dziecinne spojrzenia. Stają się szkliste, jak u umarłego. Głos, jaki dobywa ' ..
     'się z jej krtani brzmi bardziej jak z kaset nagrywanych w czasie egzorcyzmów.')
speech(name, 'Zbyt dużo stresów powoduje zawał serca. I dobrze ci będzie, zdychaj zasrańcu, ' ..
             'chciałabym, żebyś skończył jak ja!')
goto finish

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::rodzic::

speech(name, 'Rodzica? A kto to taki?')
choice({ { 'mt', 'Jak to kto? Mama i tata.' },
         { 'dobro', 'Ktoś, kto się tobą opiekuje i jest dla ciebie bardzo dobry.' },
         { 'koch', 'Ktoś, kogo bardzo kochasz.' } })

if result == 'mt' then
    goto mt
elseif result == 'dobro' then
    goto dobro
elseif result == 'koch' then
    goto koch
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::mt::

speech(name, 'Mama? Kim jest mama?')
choice({ { 'dobro', 'Ktoś, kto się tobą opiekuje i jest dla ciebie bardzo dobry.' },
         { 'koch', 'Ktoś, kogo bardzo kochasz.' } })

if result == 'dobro' then
    goto dobro
elseif result == 'koch' then
    goto koch
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::dobro::

text('Twarz dziewczynki smutnieje, po czym przebiega po niej, trwający ułamek sekundy, ' ..
     'ale niemożliwy do przeoczenia, rozpaczliwy skurcz lęku.')
speech(name, 'Cicho! Bo usłyszy! Nie ma tutaj niczego dobrego i nie mogę o tym mówić, ' ..
             'rozumie pan?')
choice({ { 'kto', 'Kto usłyszy, kogo się boisz?' },
         { 'finish', 'Spokojnie, nie musisz się bać, nic ci nie grozi.' }})

if result == 'kto' then
    goto kto
elseif result == 'finish' then
    goto finish
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::kto::

text('Ola wybicha płaczem. Łzy spływają jej po policzkach, po czym skapują na sukienkę ' ..
     'rozpuszczając plamy zakrzepłej krwi. Kiedy próbuję się do niej zbliżyć albo coś powiedzieć, ' ..
     'zatyka uszy i cofa się. Szepcze pod nosem kilka słów, przetykając je łkaniem.')
speech(name, 'Oni mi zrobią krzywdę... Pan nic nie rozumie... Proszę odejść...')
goto finish

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::koch::

speech('Ola', 'Kocham „bum”.')
goto dom

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::plecy::

text('Dziewczynka uśmiecha się niewinnie, po czym pokazuje mi to, co trzyma w dłoniach. Przez ' ..
     'moment robi mi się zimno - podsuwa mi przed oczy ogromną petardę. Kiedy zbliżam do niej ' ..
     'dłoń, chowa ją za plecami i patrzy na mnie z niesmakiem.')
speech(name, 'To moja zabawka. Moja ulubiona.')
choice({ { 'oddaj', 'Oddaj mi to natychmiast! Dzieci nie powinny bawić się takimi rzeczami.' },
         { 'skad', 'Skąd to wzięłaś?' }})

if result == 1 then
    goto oddaj
elseif result == 2 then
    goto skad
end

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::oddaj::

text('Twarz dziewczynki mnie nagły, skurcz. Przez moment nie wygląda jak dziecko, a jak jakiś ' ..
     'cholerny demon, którego jedynym sensem i treścią życia jest wściekłość. Wygląda jak bogini ' ..
     'zemsty. Po chwili złudzenie znika, ale wyraz bezmyślnej furii nadal pozostaje, co wygląda ' ..
     'grotestkowo obrzydliwie na obliczu dziecka.')
speech(name, 'Wszystko mi zabraliście, jeszcze to mi chcesz zabrać, zasrańcu?! Zabiję cię, ' ..
             'jeśli tkniesz moją zabawkę, rozumiesz?!')
text('Słowa brzmią jak zgrzytanie jakiegoś ostrego elementu o metalową powierzchnię. To nie ' ..
     'jest coś, co mogłyby wyartykułować struny głosowe małej dziewczynki. Czuję stróżkę zimnego ' ..
     'potu spływającą mi po plecach. Wolę nie ryzykować i nie próbować odebrać jej tej petardy, ' ..
     'niech ginie, czymkolwiek jest.')
goto finish

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::skad::

speech(name, 'Jest ze mną odkąd pamiętam. To jedyne, co dostałam od życia.')
text('Wydaje się nie zdawać sobie sprawy z tego, jak straszne jest to, co powiedziała. Wręcz ' ..
     'przeciwnie - zamiast posmutnieć, uśmiecha się jeszcze bardziej promiennie, a co za tym ' ..
     'idzie, staje się jeszcze bardziej przerażająca. Teraz widać tylko jej ogromny, pełen jakiejś ' ..
     'złośliwej satysfakcji uśmiech przytwierdzony do wątłego, maleńkiego ciała.')
goto finish

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

::finish::

dialogue_end()
