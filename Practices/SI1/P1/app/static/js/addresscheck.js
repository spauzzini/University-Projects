$(document).ready(function(){
    $("#address").keyup(function(){
        var p = $("#address").val();
        var l = p.length;
        if (l<=50){
            $("#avisoadd").css('visibility', 'hidden')
        }else {
            $("#avisoadd").css('visibility', 'visible')
        }

    })
});