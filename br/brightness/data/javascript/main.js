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
    ctx.fillStyle = "#000000";
    ctx.fill();
  } else {
    ctx.fillStyle = "#FFFFFF";
    ctx.fill();
  }
}

function light_toggle() {
  if (canvas.class == "0") {
    do_request("1");
  } else {
    do_request("0");
  }
}

function do_request(value) {
  if (null == value) { value = "get"; }
  http.open("GET", "/?value=" + value, true);
  http.send(null);
}

setInterval(do_request, 1000);
