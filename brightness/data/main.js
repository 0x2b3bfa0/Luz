function report(value) {
  var http=new ajaxRequest()
  http.onreadystatechange=function(){
   if (http.readyState==4){
    if (http.status==200 || window.location.href.indexOf("http")==-1){
     // document.getElementById("result").innerHTML=http.responseText
    }
    else{
     // Smart way to notify the user that it forgot to plug the device
     window.location.reload()
     // alert("An error has occured making the request")
    }
   }
  }
  http.open("GET", "/?brightness=" + value, true)
  http.send(null)
}
