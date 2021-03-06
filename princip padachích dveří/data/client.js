let index = 0;
$(function () {

    /* Funkce, která přidá skupinu prvků do formuláře */
    function addGroup(index = 0, obj = { cas: '', davka: '', datum: '' }) {
        return `
        <div class="form-group row p-1 group-time" id="group-${index}">
          <label class="col-sm-1">Cas krmeni</label>
          <div class="col-sm-2">
              <input type="time" class="form-control" required name="cas" id="hodiny${index}" value="${obj.cas}">
              <p> Davka zacne vzdy na zacatku minuty <\p> 
          </div>
          <label class="col-sm-1">Velikost davky</label>
          <div class="col-sm-2">
              <input type="number" class="form-control" required name="davka" id="davka${index}" value="${obj.davka}" min="1" max="5">
              <p> Nastaveni davky (1-5) od nejmensi<\p>
          </div>

          <label class="col-sm-1">Den</label>
          <div class="col-sm-3">
          <select name="dny" id="datum${index}">
          <option value="Monday">Pondeli</option>
          <option value="Tuesday">Utery</option>
          <option value="Wednesday">Streda</option>
          <option value="Thursday">Ctvrtek</option>
          <option value="Friday">Patek</option>
          <option value="Saturday">Sobota</option>
          <option value="Sunday">Nedele</option>
        </select>
        <p> Dny se musi shodovat!<\p>
          </div>
          <div class="col-sm-2">
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
        index++;
        /* Index se po přidání prvku zvýší, aby se zajistila jeho unikátnost */
    });

    /* Pole pro uložení dat z formuláře */
    let data = [];
    /* Načtení dat ze serveru */
    get();

})


//Funkce pro jednoduché přemazávání starých proměnných, takže při každém aktualizování dat je zajištěno i smazání dat starých.
$('#send').on('click', function () {
    let i = 0;
    for (i = 0; i < index; i++) {
        var ho = "-1";
        var dd = "-1";
        var da = "-1";
        console.log(i);
        console.log(ho);
        console.log(dd);
        console.log(da); 
        $.post("/hodiny", { hodiny: ho });
        $.post("/davka", { davka: dd });
        $.post("/datum", { datum: da });
        $.post("/i", { i, i });
    }


    //cyklus for pro zajisteni zaslani všech prvků formuláře
    for (i = 0; i < index; i++) {
        //získání hodnoty z daného prvku formuláře a uložení teto hodnoty do proměnné ho
        var ho = $('#hodiny' + i).val();
        var dd = $('#davka' + i).val();
        var da = $('#datum' + i).val();
        console.log(i);
        //vypsání do konzole na webu (zapíná se F12)
        console.log(ho);
        console.log(dd);
        console.log(da);
        //zaslání proměnné ho na esp kde se díky funkci server.on zpracuje 
        $.post("/hodiny", { hodiny: ho });
        $.post("/davka", { davka: dd });
        $.post("/datum", { datum: da });
        $.post("/i", { i, i });
    }
});