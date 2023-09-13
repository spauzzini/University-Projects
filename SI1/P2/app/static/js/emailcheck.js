$(document).ready(function(){
    $("#email").keyup(function(){
        var p = $("#email").val();
        var regex = /^([a-zA-Z0-9_.+-])+\@(([a-zA-Z0-9-])+\.)+([a-zA-Z0-9]{2,4})+$/;

        if(regex.test(p)){
            $("#avisoemail").css('visibility', 'hidden')
        }else{
            $("#avisoemail").css('visibility', 'visible')
        }
    })
});