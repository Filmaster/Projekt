  function ulozCas() {
  var ho = $('#hodiny').val();
  var dd = $('#davka').val();
  var da = $('#datum').val();
  $.post("/hodiny", { hodiny: ho });
  $.post("/davka", { davka: dd });
  $.post("/datum", { datum: da });
}