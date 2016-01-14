var http=new ajaxRequest();

http.onreadystatechange=function() {
  if (http.readyState==4){
    if (http.status==200 || window.location.href.indexOf("http")==-1) {
      var respose = http.responseText;
      if (response != "OK") {
        document.getElementById("brightness").value = http.responseText;
      }
    } else {
      // Smart way to notify the user that there is a connection error ;-)
      window.location.reload();
    }
  }
}

function request(value) {
  if (null == value) { value = "get"; }
  http.open("GET", "/?value=" + value, true);
  http.send(null);
}

setInterval(request, 1000);
