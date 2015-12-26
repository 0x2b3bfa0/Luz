var http=new ajaxRequest();

http.onreadystatechange=function() {
 if (http.readyState==4){
  if (http.status==200 || window.location.href.indexOf("http")==-1){
   if (http.responseText == "0") {
     canvas.class="0";
   } else {
     canvas.class="1";
   }
   color_toggle();
  }
  else {
   // Smart way to notify the user that it forgot to plug the device
   window.location.reload();
  }
 }
}

function color_toggle() {
  if (canvas.class == "0") {
    ctx.fillStyle = "#FFFFFF";
    ctx.fill();
  } else {
    ctx.fillStyle = "#000000";
    ctx.fill();
  }
}

function light_toggle() {
  if (canvas.class == "0") {
    value = 1;
    canvas.class = "1";
    http.open("GET", "/?value=0", true);
    http.send(null);
  } else {
    value = 0;
    canvas.class = "0";
    http.open("GET", "/?value=1", true);
    http.send(null);
  }
}
