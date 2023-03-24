dialogue_start()

if sanity >= 75 then
    text('W garnku znajduje się jakaś cholerna, bezkształtna breja. Pachnie jak gówno. Hm... Wygląda ' ..
         'w sumie też jak gówno. Ciekawe jak smakuje...')
elseif sanity >= 50 then
    text('Szlag! Co to ma znaczyć?! Czemu w garnku widzę poćwiartowanego szczura wielkiego jak kot? Zaraz ' ..
         'się zerzygam, z odciętej głowy zwierzęcia spływa strużka krwi, do której zbiegają się jakieś ' ..
         'obrzydliwe karaluchy.')
elseif sanity >= 25 then
    text('Zbliżam dłoń do zawartości garnka, a ona, jakby wyczuła mój ruch, nagle ożywa. Miliony małych, ' ..
         'tłustych, białych rokabów wiją się w miejscu, tworząc obrzydliwą, ruchliwą masę. Sam widok tego ' ..
         'syfu sprawia, że mam ochotę puścić pawia, a potem uświadamiam sobie, że to właśnie jest nasze ' ..
         'jedzenie. Żołądek podchodzi mi do gardła. Z pewnością opróżniłbym go, gdyby już nie był pusty. ' ..
         'A pewnie pozostanie jeszcze taki dość długo.')
else
    text('O nie, tego już za wiele! Chcą z nas zrobić cholernych kanibali. Choć nie wiem, czy to akurat ' ..
         'jest w tej sytuacji najgorsze - w garnku widzę poćwiartowanego człowieka. Coś dłuiego, chyba jelito, ' ..
         'wystaje z naczynia i zwiesza się ze stolika aż do samej podłogi. Bezmyślne, szkiste oczy wpatrują ' ..
         'się we mnie. Poszarzała, wyglądająca na chorą skóra - dokładnie taka, jaką mają mieszkańcy schronu - ' ..
         'w wielu miejscach poznaczona jest krwawymi kleksami. W pomieszczeniu śmierdzi padliną, wymiotami ' ..
         'i czymś jeszcze, co ciężko mi określić. Jakiś mdły, boleśnie raniący nos odór...')
end

dialogue_end()
