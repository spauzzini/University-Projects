$(document).ready(function(){
    $("#password").keyup(function(){
        var p = $("#password").val();
        var l = p.length;
        if (l==0){
            $("#lenght").val(0)
        }
        else if (l==1){
            $("#lenght").val(16)
            $('#bajo').css('visibility','hidden')
        } else if(l==2){
            $("#lenght").val(33)
            $('#bajo').css('visibility','hidden')
        } else if(l==3){
            $("#lenght").val(50)
            $('#bajo').css('visibility','hidden')
        } else if(l==4){
            $("#lenght").val(66)
            $('#bajo').css('visibility','hidden')
        } else if(l==5){
            $("#lenght").val(83)
            $('#bajo').css('visibility','hidden')
        }
        else{
            $("#lenght").val(100)
            if(l==6){
                $('#bajo').css('visibility','visible')
                $('#medio').css('visibility','hidden')
                $('#alto').css('visibility','hidden')
                $('#malto').css('visibility','hidden')
            }else if(l==7){
                $('#bajo').css('visibility','hidden')
                $('#medio').css('visibility','visible')
                $('#alto').css('visibility','hidden')
                $('#malto').css('visibility','hidden')
            }else if(l==8 || l==9){
                $('#bajo').css('visibility','hidden')
                $('#medio').css('visibility','hidden')
                $('#alto').css('visibility','visible')
                $('#malto').css('visibility','hidden')
            }else{
                $('#bajo').css('visibility','hidden')
                $('#medio').css('visibility','hidden')
                $('#alto').css('visibility','hidden')
                $('#malto').css('visibility','visible')
            }
        }
    })
});