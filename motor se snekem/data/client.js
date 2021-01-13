let index = 0;
$(function () {
    /* Inicializace proměnné index, která unikátně identifikuje novou skupinu prvků */

    /* Funkce, která přidá skupinu prvků do formuláře */
    function addGroup(index = 0, obj = { cas: '', davka: '', datum: '' }) {
        return `
        <div class="form-group row p-1 group-time" id="group-${index}">
          <label class="col-sm-1">Cas krmeni</label>
          <div class="col-sm-2">
              <input type="time" class="form-control" required name="cas" id="hodiny${index}" value="${obj.cas}">
          </div>
          <label class="col-sm-1">Velikost davky</label>
          <div class="col-sm-2">
              <input type="number" class="form-control" required name="davka" id="davka${index}" value="${obj.davka}" min="1" max="5">
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

function get() {
    let i = 0;
    for (i = 0; i < index; i++) {
        var hoG = $('#hodiny' + i).val();
        var ddG = $('#davka' + i).val();
        var daG = $('#datum' + i).val();
        console.log(iG);
        console.log(hoG);
        console.log(ddG);
        console.log(daG);
        $.get("/hodiny", { hodiny: ho });
        $.get("/davka", { davka: dd });
        $.get("/datum", { datum: da });
        $.get("/i", { i, i });
    }
}






$('#send').on('click', function () {
    let i = 0;
    for (i = 0; i < index; i++) {
        var ho = $('#hodiny' + i).val();
        var dd = $('#davka' + i).val();
        var da = $('#datum' + i).val();
        console.log(i);
        console.log(ho);
        console.log(dd);
        console.log(da);
        $.post("/hodiny", { hodiny: ho });
        $.post("/davka", { davka: dd });
        $.post("/datum", { datum: da });
        $.post("/i", { i, i });
    }
});

$('#get').on('click', function () {
    let i = 0;
    for (i = 0; i < index; i++) {
        var hoG = $('#hodiny' + i).val();
        var ddG = $('#davka' + i).val();
        var daG = $('#datum' + i).val();
        console.log(i);
        console.log(ho);
        console.log(dd);
        console.log(da);
        $.get("/hodiny", { hodiny: ho });
        $.get("/davka", { davka: dd });
        $.get("/datum", { datum: da });
        $.get("/i", { i, i });
    }
});