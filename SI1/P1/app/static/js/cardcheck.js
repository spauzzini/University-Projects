$(document).ready(function(){
    $("#card").keyup(function(){
        var p = $("#card").val();
        var l = p.length;
        if (l==16){
            $("#avisocard").css('visibility', 'hidden')
        }else {
            $("#avisocard").css('visibility', 'visible')
        }

    })
});