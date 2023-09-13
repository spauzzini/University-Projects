$(document).ready(function(){
    $("#username").keyup(function(){
        var p = $("#username").val();
        var l = p.length;
        const specialChars = /[`!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?~]/;

        if (l>=5){
            $("#avisoregus5").css('visibility', 'hidden')
        }else if (l<5){
            $("#avisoregus5").css('visibility', 'visible')
        }

        if(p.indexOf(' ')>=0){
            $("#avisoespacio").css('visibility', 'visible')
        }else if (p.indexOf(' ')<0){
            $("#avisoespacio").css('visibility', 'hidden')
        }

        if (specialChars.test(p)){
            $("#avisospec").css('visibility', 'visible')
        }
        else if(specialChars.test(p) == false){
            $("#avisospec").css('visibility', 'hidden')
        }

    })
});