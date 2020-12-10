$(function(){
    var i = 0;
    function zmenaTextu(i){
        $('#rozvrh article h4').text(rozvrh[i].den);
        $('#rozvrh article p').text(rozvrh[i].cas);
        $('#rozvrh article h2').text(rozvrh[i].davka);
    }

    var a = 0;
    zmenaTextu(a);
    $('#rozvrh button').on('click',function(){
        a < rozvrh.length - 1 ? a++ : a = 0;
        zmenaTextu(a);
    })
})

$(".deleteMe").on("click", function(){
    $(this).closest("div").remove(); 
 });