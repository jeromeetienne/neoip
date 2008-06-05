function magnetGetQueryString() {
   var uri = document.location + "";
   var queryPart = uri.substring(uri.indexOf('?')+1,uri.length);
   return queryPart;
}

function magnetOpenOptionsPage(uri) {
    var queryPart = uri.substring(uri.indexOf("?")+1,uri.length);
    document.location="magnet-uri-template.html?"+queryPart;
}

function magnetHijackClick(ev) {
    var uri;
    if((ev!=null)&&(ev.preventDefault!=null)) {
       ev.preventDefault();
       uri = this.href;
    } else {
       event.returnValue=false;
       uri = event.srcElement.href;
    }
    magnetOpenOptionsPage(uri);
}
function magnetPatchLinks() {
   for (var i=0; i<=(document.links.length-1); i++)
   {
       if(document.links[i].href.indexOf('magnet:')==0) {
           document.links[i].onclick=magnetHijackClick;
       }
   }
}

// setup for any page with magnet: URI hrefs
if(document.body.onload==null) {
  document.body.onload=magnetPatchLinks;
}

// setup for template page
var magnetQueryString=magnetGetQueryString();

var magnetMaxSlot=50;
var magnetResultCompletions=0;
var magnetPollResultArray= new Array();
var magnetOptionsPollSuccesses=0;
var magnetCurrentSlot=-1;
var magnetOptionsPollMaxFaults=3
var magnetOptionsPreamble="<hr>\n";
var magnetOptionsPostamble="\n"

function magnetDoOptionsPoll(queryString) {
   magnetCurrentSlot++;
   if (magnetCurrentSlot>magnetMaxSlot) {
     // checked them all
     return;
   }
   if ((magnetCurrentSlot-magnetOptionsPollSuccesses)>magnetOptionsPollMaxFaults) {
     // too many open slots, stop checking
     return;
   }
   // otherwise, do a single poll
   document.write("<script src=\"http://127.0.0.1:"+(9080+magnetCurrentSlot)+
                  "/magnet10/options.js?"+magnetQueryString+"\"><\/script>\n");
}