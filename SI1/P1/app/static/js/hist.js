$(document).ready(function(){
    $("#titl").hover(function(){
        $(cont).css('visibility', 'visible')
        })
    $("#titl").mouseleave(function(){
        $(cont).css('visibility', 'hidden')
        })
});