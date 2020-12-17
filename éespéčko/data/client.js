$(function () {
    /* Inicializace proměnné index, která unikátně identifikuje novou skupinu prvků */
    let index = 0;
    /* Funkce, která přidá skupinu prvků do formuláře */
    function addGroup(index = 0, obj = { cas: '', davka: '', datum: '' }) {
        return `
        <div class="form-group row p-1 group-time" id="group-${index}">
            <label class="col-sm-1">Cas krmeni</label>
            <div class="col-sm-2">
                <input type="time" class="form-control" required name="cas" id="cas-${index}" value="${obj.cas}">
            </div>
            <label class="col-sm-1">Velikost davky</label>
            <div class="col-sm-2">
                <input type="number" class="form-control" required name="davka" id="davka-${index}" value="${obj.davka}" min="1" max="5">
            </div>
            <label class="col-sm-1">Den</label>
            <div class="col-sm-3">
                <input type="date" class="form-control" required name="datum" id="datum-${index}" value="${obj.datum}">
            </div>
            <div class="col-sm-2">
                <button type="button" class="btn btn-success poslat" id="poslat-${index}">Odeslat</button>
                <button type="button" class="btn btn-danger delete" id="delete-${index}">Smazat</button>
            </div>
        </div>
        `;
    }

    /* Akce po kliknutí na tlačítko Přidat nový čas */
    $('#append').on('click', function () {
        /* Přidá skupinu prvků jako nový blok do formuláře */
        $('#times').append(addGroup(index));
        /* Ošetření akce kliknutí na tlačítko Smazat - smaže se vybraná skupina prvků */
        $('.delete').on('click', function () {
            /* Příklad traverzování - vyhledá se a odstraní celý element - 
            předek tlačítka Smazat, který je oddílem s třídou group-time */
            $(this).parents('div.group-time').remove();
        });
        /* Index se po přidání prvku zvýší, aby se zajistila jeho unikátnost */
        index++;
    });

    /* Pole pro uložení dat z formuláře */
    let data = [];

    /* Validační funkce pro ověření platnosti zadaných údajů */
    function validateData(obj) {
        /* Z obou časových stringů odebere dvojtečku a porovná, zda je konečný údaj větší */
        /* Musí být zadáno také neprázdné datum */
        return (obj.cas.replace(':', '') < obj.davka.replace(':', '')) && obj.datum;
    }

    


    /* Načtení dat ze serveru */
    get();

})
