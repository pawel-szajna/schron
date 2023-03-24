dialogue_start()

text('Mężczyzna przygląda mi się badawczo, jakby próbując zorientować się, czy już gdzieś się wcześniej nie ' ..
     'widzieliśmy, po czym usmiecha się.')
speech('Lekarz', 'Cześć. Coś ci dolega?')

choice({ {'sanit', 'Szczerze mówiąc, tak... Co prawda nie fizycznie, ale psychicznie nie czuję się najlepiej.'},
         {'co',    'Nie, ale chciałbym się dowiedzieć, co się tutaj w ogóle dzieje.'},
         {'rozm',  'Szczerze mówiąc, to chciałem tyko porozmawiać z kimś normalnym. W tym domu wariatów ty ' ..
                   'wydajesz się być jednym z normalniejszych.'} })

dialogue = {
    sanit = function()
        speech('Lekarz', 'Co prawda nie jestem psychiatrą, ale może uda mi się coś poradzić, kiedyś trochę ' ..
                         'się tym zajmowałem.')
        if sanity < 25 then
            speech('Lekarz', 'No, chłopie, stąpasz po krawędzi. Postaraj się unikać stresów, bo nie za dobrze ' ..
                             'z tobą. Możesz mieć problemy z odróżnieniem swoich wyobrażeń od tego, co rzeczywiście ' ..
                             'widzą twoje oczy, ale to najmniejszy kłopot. Gdybyś kiedykolwiek miał jakies... ' ..
                             'hmmm... niepokojące myśli, gdybyś miał ochotę zrobić sobie albo komuś krzywdę, wiesz ' ..
                             'gdzie mnie szukać. Wiele nie zdziałam, ale przynajmniej mogę ci podrzucić tabletkę.')
        elseif sanity < 50 then
            speech('Lekarz', 'Uważaj na siebie. Z twoją głową nie jest najlepiej, ale też nie warto przejmować się ' ..
                             'na zapas, po prostu postaraj się jakoś rozładować napięcie i pod żadnym pozorem nie ' ..
                             'rób niczego, co mogłoby jeszcze bardziej pogorszyć twój stan psychiczny.')
        elseif sanity < 75 then
            speech('Lekarz', 'Widać u ciebie objawy przewekłego stresu i lekkie zaburzenia w postrzeganiu ' ..
                             'rzeczywistości, ale to nic, czym musiałbyś się przejmować bardziej niż atakiem ' ..
                             'terrorystycznym, przez który jesteś tutaj zamknięty. Szczerze mówiąc, po całym ' ..
                             'tygodniu pracy sam czasem mam podobne objawy.')
        else
            speech('Lekarz', 'Psychicznie jesteś zdrowy, o ile istnieje coś takiego jak zdrowie psychiczne. Może ' ..
                             'wszyscy mamy poprzestawiane klepki, kto to wie... Ale według mojej wiedzy nie masz ' ..
                             'się czym martwić, choć nie znaczy to oczywiście, że jak nie będziesz się szanował, ' ..
                             'nie będzie gorzej.')
        end
        result = 'end'
    end,
    co = function()
        text('Mężczyzna wydaje się rozbawiony moim pytaniem. Uśmiecha się, kręcąc głową z politowaniem.')
        speech('Lekarz', 'Jest wiele odpowiedzi, zależy którą z nich chcesz usłyszeć.')
        choice({ {'wytlum', 'Daruj sobie te filozoficzne bzdury, dookoła dzieją się popieprzone rzeczy, nie ' ..
                            'możesz tego nie widzieć. Chcę, żeby ktoś mi wreszcie wytłumaczył.'},
                 {'wygl', 'Chodzi mi głównie o to, czemu wszyscy tak dziwnie wyglądają i się tak dziwnie zachowują.'},
                 {'odpo', 'A którą chciałbyś mi zdradzić?'} })
    end,
    wytlum = function()
        text('Lekarz uśmiecha się radośnie. Wygląda na to, że moja bezsilność jest dla niego szalenie zabawna. ' ..
             'Zaciskam pięści, żeby nie pokazać mu, jak bardzo jestem wściekły.')
        speech('Lekarz', 'A jak miałbym ci wytłumaczyć coś, czego sam nie rozumiem? Świat jest na tyle dziwny, ' ..
                         'że tak naprawdę niczego nie da się wytłumaczyć, albo wytłumaczeń jest tyle, ile ludzi. ' ..
                         'Kwestią jest to, w co tak naprawdę chcesz uwierzyć.')
        choice({ {'dupa', 'To, w co wierzę nie zmieni tego, że jestem w dupie.'},
                 {'malowie', 'Rozumiem więc, że sam wiesz o tym co się dzieje równie mało jak ja, czyż nie?'} })
    end,
    dupa = function()
        speech('Lekarz', 'Właśnie... Wszystko zależy od tego, jak definiujesz „bycie w dupie”. To, co dla jednego ' ..
                         'jest totalnym gównem, dla innego będzie tabliczką czekolady, jeśli wiesz co mam na myśli. ' ..
                         'Nie ma czegoś takiego jak gówno obiektywne, bo obiektywna rzeczywistość w istocie nie ' ..
                         'istnieje.')
        choice({ {'coistnieje', 'Skoro obiektywna rzeczywistość nie istnieje, to jaka istnieje?'},
                 {'end', 'Słuchaj, mam dość tego pieprzenia. Chciałem się dowiedzieć, w co się wpadkowałem, ' ..
                         'ale jak widzać ty mi w tym nie pomożesz. Do widzenia.'} })
    end,
    coistnieje = function()
        text('Niemal podskakuje z radości. Po jego minie widzę, że właśnie na to pytanie czekał.')
        speech('Lekarz', 'Istnieje to, co odczuwamy. Jeśli widzisz ogień, jeśli czujesz jego ciepło, on ' ..
                         'istnieje, jest na tyle realny, żeby cię poparzyć. Jeśli nie, ognia nie ma, nawet ' ..
                         'jeśli ktoś twierdzi, że go rozpalił. Jeśli dla ciebie coś jest zimne, ale dla mnie ' ..
                         'gorące, to jakie jest tak naprawdę?')
        choice({ {'prawdynie', 'Eee... Ani jedno ani drugie?'},
                 {'prawdatak', 'To proste, wystarczy zmierzyć.'} })
    end,
    prawdatak = function()
        text('Lekarz krzywi się, jakby właśnie zjadł coś niesmacznego. Albo usłyszał. Przez chwilę wydaje się ' ..
             'zastanawiać nad tym, co odpowiedzieć, ale równie dobrze może to być duszenie złości, jaką wyzwoliły ' ..
             'moje słowa. Ale nie negatywnej, ale takiej, jaką może czuć nauczyciel do wyjątkowo niepojętnego ucznia.')
        speech('Lekarz', 'Dobrze więc, zmierz. Jeden termometr wskaże dwadzieścia stopni, drugi piętnaście. Który ' ..
                         'ma rację?')
        choice({ {'prawdynie', 'Żaden?'},
                 {'end', 'Po prostu napraw ten zepsuty. Nie mam ochoty dalej gadać o pierdołach. Na razie.'} })
    end,
    prawdynie = function()
        text('Na twarzy lekarza pojawia się szeroki, pełen dumy uśmiech. Nie wiem czy jest dumny bardziej ze ' ..
             'mnie, że pojąłem co miał mi do powiedzenia, czy z siebie, że przekonał mnie do tych bzdur.')
        speech('Lekarz', 'Właśnie. Nic nie może być obiektywne w świecie subiektywnych bytów. Tylko i wyłącznie ' ..
                         'percepcja sprawia, że rzeczy są takie, jakimi się wydają.')
        choice({ {'rozm', 'Rozumiem... Może teraz porozmawiamy o czymś innym, od dawna nie udało mi się z nikim ' ..
                          'sensownie porozumieć.'},
                 {'end', 'Niech będzie, że się zgadzam. Teraz wybacz, ale mam coś do zrobienia.'},
                 {'end', 'Nie będę dalej słuchał o tych cholernych bzdruach. Żegnam.'} })
    end,
    wygl = function()
        text('Lekarz wzrusza ramionami, a na jego twarzy maluje się przepraszający usmiech.')
        speech('Lekarz', 'Świat jest dziwny. Choć z drugiej strony, czemu myślisz, że to oni się dziwnie ' ..
                         'zachowują, może to z tobą coś nie tak.')
        choice({ {'sanit', 'Może i racja. Ostatnio nie czuję się najlepiej, chciałbym, nie wiem, czy nie dzieje ' ..
                           'się ze mną coś złego...'},
                 {'szale', 'Masz mnie za szaleńca?'},
                 {'end', 'To z tobą jest coś nie tak. Spadaj.'} })
    end,
    szale = function()
        speech('Lekarz', 'Nie bardziej niż siebie.')
        result = 'end'
    end,
    odpo = function()
        speech('Lekarz', 'Taki sposób myślenia mi się podoba! Cóż, ja mogę ci powiedzieć wszystko, ' ..
                         'ale tak naprawdę znaczenie ma to, w co ty zechcesz uwierzyć.')
        choice({ {'dupa', 'To, w co wierzę nie zmieni tego, że jestem w dupie.'},
                 {'malowie', 'Rozumiem więc, że sam wiesz o tym co się dzieje równie mało jak ja, czyż nie?'} })
    end,
    malowie = function()
        speech('Lekarz', 'Oczywiście. Równie mało jak i równie dużo.')
        result = 'end'
    end,
    rozm = function()
        text('TODO')
        result = 'end'
    end
}

while (result ~= 'end') do
    dialogue[result]()
end

dialogue_end()
