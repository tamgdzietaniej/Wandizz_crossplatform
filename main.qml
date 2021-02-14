import QtQuick 2.0
import QtQuick.LocalStorage 2.12
import QtWebView 1.9
Item {
id: web
x:0
y:0
width: w
height: h
visible: true;
signal completed(msg: string)
signal undef()
WebView {
function getSource(){
var js = "document.documentElement.getElementsByTagName('json')[0].innerHTML";
webView.runJavaScript(js, function(result){
console.log("RESULT="+result+"wh"+w+","+h);
if(result === undefined){
web.visible = true
webView.visible=true
web.undef()
} else {
web.visible=false
webView.visible=false;
web.completed(result)
}

});
}
id: webView
visible:false;
x:0
y:0
width: web.width
height: web.height
anchors.fill: parent;
url: ref
clip: false
antialiasing: true
onLoadingChanged: {
if(loadProgress.valueOf()==100){
getSource();
console.log("100%, w:"+web.width+"h:"+web.height+"wh:"+webView.height);
}
}
onUrlChanged: {
console.log("URL:"+url);
}
}
}

/*
var st="var css=\"background: rgba(88,59,128,1);background: -moz-linear-gradient(left, rgba(88,59,128,1) 0%, rgba(180,2,200,1) 48%, rgba(88,59,128,1) 100%);
background: -webkit-gradient(left top, right top, color-stop(0%, rgba(88,59,128,1)), color-stop(48%, rgba(180,2,200,1)), color-stop(100%, rgba(88,59,128,1)));
background: -webkit-linear-gradient(left, rgba(88,59,128,1) 0%, rgba(180,2,200,1) 48%, rgba(88,59,128,1) 100%);
background: -o-linear-gradient(left, rgba(88,59,128,1) 0%, rgba(180,2,200,1) 48%, rgba(88,59,128,1) 100%);
background: -ms-linear-gradient(left, rgba(88,59,128,1) 0%, rgba(180,2,200,1) 48%, rgba(88,59,128,1) 100%);
background: linear-gradient(to right, rgba(88,59,128,1) 0%, rgba(180,2,200,1) 48%, rgba(88,59,128,1) 100%);
filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#583b80', endColorstr='#583b80', GradientType=1 );\";
*/
