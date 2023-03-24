dialogue_start()

if not met_cook then
    text('Kucharz świdruje mnie swoimi świńskimi oczkami. Wydaje z siebie jakieś dziwne odgłosy przypominające ' ..
         'chrząknięcia, jakie mógłby wydawać z siebie ktoś, kto chce oczyścić gardło z zalegającej wydzieliny. ' ..
         'Najwyraźniej mam potraktować to jak zaproszenie do rozmowy.')
    met_cook = true
end

choice({{'chrum', '(Pochrząkam)'},
        {'cona', 'Co dzisiaj na obiad?'},
        {'cosie', 'Mógłbyś mi przybliżyć, co tu się dzieje?'}})

if result == 'chrum' then
    text('Kucharz odpowiada podobnymi odgłosami, po czym zmienia repertuar na gardłowe pomruki. Przez chwilę ' ..
         'zastanawiam się, czy rzeczywiście coś znaczą, po czym odpuszczam sobie i kończę tę bezsensowną ' ..
         '„rozmowę”.')
elseif result == 'cona' then
    speech('Kucharz', 'To, co widzisz w garach. Nie pasuje, to gotuj se sam.')
    text('Słowa wypluwane przez kucharza toną w dziwnych dźwiękach, jakie wydaje przy okazji oddychania. ' ..
         'Musi być mu naprawdę ciężko.')
else
    speech('Kucharz', 'To co zwykle. Banda wygłodniaych darmozjadów czeka aż pzygotuję im wyżerkę. Idź na ' ..
                      'swoje miejsce, mam kupę roboty.')
    text('Każdy oddech jest dla kucharza ogromnym wysiłkiem, więc może lepiej, choćby z czystego współczucia, ' ..
         'spełnić jego polecenie.')
end

dialogue_end()
