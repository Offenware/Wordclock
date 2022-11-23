var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    websocket.send("states");
}
  
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
} 

// Hier komt nieuwe data binnen, dus de pagina aanpassen
function onMessage(event) {
    var myObj = JSON.parse(event.data);
    console.log(myObj);
    
    if(myObj.hasOwnProperty('maanfase')){
      document.getElementById("tijd").innerHTML = myObj.tijd;
      document.getElementById("zonop").innerHTML = myObj.zonop;
      document.getElementById("zononder").innerHTML = myObj.zononder;
      document.getElementById("temperatuur").innerHTML = myObj.temperatuur;
      document.getElementById("druk").innerHTML = myObj.druk;
      document.getElementById("stap").innerHTML = myObj.stap;
      document.getElementById("sterkte").innerHTML = myObj.sterkte;
      document.getElementById("maanfase").innerHTML = myObj.maanfase;
    } 
    
    if(myObj.hasOwnProperty('mode')){
      document.getElementById("mode").value = myObj.mode;
      document.getElementById("ipadres").innerHTML = myObj.ipadres;
      document.getElementById("minBrightness").value = myObj.minBrightness;
      document.getElementById("maxBrightness").value = myObj.maxBrightness;
      document.getElementById("stepBrightness").value = myObj.stepBrightness;
      document.getElementById("startBrightness").value = myObj.startBrightness;
      
      document.getElementById("kleurRood").value = myObj.rood;
      document.getElementById("kleurGroen").value = myObj.groen;
      document.getElementById("kleurBlauw").value = myObj.blauw;

      document.getElementById("kleurSRood").value = myObj.srood;
      document.getElementById("kleurSGroen").value = myObj.sgroen;
      document.getElementById("kleurSBlauw").value = myObj.sblauw;

      document.getElementById("lengte").value = myObj.lengtegraad;
      document.getElementById("breedte").value = myObj.breedtegraad;
     
      document.getElementById("uuruit").value = myObj.uuruit;
      document.getElementById("uuraan").value = myObj.uuraan;
      document.getElementById("nachtsterkte").value = myObj.nachtsterkte;
      
      document.getElementById("maan").checked = myObj.maan;
      document.getElementById("desOchtends").checked = myObj.desOchtends;
      document.getElementById("hetIs").checked = myObj.hetIs;
      
      document.getElementById("zomer").checked = myObj.zomer;
      document.getElementById("timeserver").value = myObj.timeserver;
      document.getElementById("timeoffset").value = myObj.timeoffset;
      
      const rbs1 = document.querySelectorAll('input[name="wordclock"]');
      for (const rb1 of rbs1) {
        if (rb1.value == myObj.wordclock)  {
           rb1.checked = true;
           break;
        }
      }
      const rbs2 = document.querySelectorAll('input[name="effect"]');
      for (const rb2 of rbs2) {
        if (rb2.value == myObj.effect)  {
           rb2.checked = true;
           break;
        }
      }
    }
    
    console.log(event.data);
}

// Send Requests to Control Mode
function handleZomer (element) {
  var zomer = element.checked; 
  console.log(zomer);
  var obj = { kleurmode: "zomer", zomer: zomer };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("zomer").checked = zomer;
}

function handleTimeserver (element) {
  var timeserver = element.value; 
  console.log(timeserver);
  var obj = { kleurmode: "timeserver", timeserver: timeserver.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("timeserver").value = timeserver.toString();
}

function handleHetIs (element) {
  var hetIs = element.checked; 
  console.log(hetIs);
  var obj = { kleurmode: "hetIs", hetIs: hetIs };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("hetIs").checked = hetIs;
}

function handleDesOchtends (element) {
  var desOchtends = element.checked; 
  console.log(desOchtends);
  var obj = { kleurmode: "desOchtends", desOchtends: desOchtends };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("desOchtends").checked = desOchtends;
}

function handleMaan (element) {
  var maan = element.checked; 
  console.log(maan);
  var obj = { kleurmode: "maan", maan: maan };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("maan").checked = maan;
}

function handleWordclock (element) {
  const rbs = document.querySelectorAll('input[name="wordclock"]');
  let selectedValue;
  for (const rb of rbs) {
    if (rb.checked) {
       selectedValue = rb.value;
       break;
    }
  }
  console.log(selectedValue);
  var obj = { kleurmode: "wordclock", wordclock: selectedValue };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  const rbs1 = document.querySelectorAll('input[name="wordclock"]');
  for (const rb1 of rbs1) {
    if (rb1.value == selectedValue)  {
       rb1.checked = true;
       break;
    }
  }
}

function handleUurUit (element) {
  var uuruit = element.value; 
  console.log(uuruit);
  var obj = { kleurmode: "uuruit", uuruit: uuruit.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("uuruit").value = uuruit.toString();
}

function handleUurAan (element) {
  var uuraan = element.value; 
  console.log(uuraan);
  var obj = { kleurmode: "uuraan", uuraan: uuraan.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("uuraan").value = uuraan.toString();
}

function handleNachtsterkte (element) {
  var nachtsterkte = element.value; 
  console.log(nachtsterkte);
  var obj = { kleurmode: "nachtsterkte", nachtsterkte: nachtsterkte.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("nachtsterkte").value = nachtsterkte.toString();
}

function handleEffect (element) {
  const rbs = document.querySelectorAll('input[name="effect"]');
  let selectedValue;
  for (const rb of rbs) {
    if (rb.checked) {
       selectedValue = rb.value;
       break;
    }
  }
  console.log(selectedValue);
  var obj = { kleurmode: "effect", effect: selectedValue };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  const rbs1 = document.querySelectorAll('input[name="effect"]');
  for (const rb1 of rbs1) {
    if (rb1.value == selectedValue)  {
       rb1.checked = true;
       break;
    }
  }
}

function handleDropdown (element) {
  console.log(element.value);
  var klokmode = element.value; 
  var obj = { klokmode: element.value };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("mode").value = klokmode;
}

function handleDropdownOffset (element) {
  var timeoffset = element.value; 
  console.log(timeoffset);
  var obj = { kleurmode: "timeoffset", timeoffset: timeoffset.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("timeoffset").value = timeoffset.toString();
}

function handleLengte (element) {
  var lengte = element.value; 
  console.log(lengte);
  var obj = { kleurmode: "lengtegraad", lengtegraad: lengte.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("lengte").value = lengte.toString();
}

function handleBreedte (element) {
  var breedte = element.value; 
  console.log(breedte);
  var obj = { kleurmode: "breedtegraad", breedtegraad: breedte.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("breedte").value = breedte.toString();
}

function handleMinBrightness (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 11
  }
  var obj = { kleurmode: "minBrightness", minBrightness: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("minBrightness").value = br.toString();
}

function handleMaxBrightness (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "maxBrightness", maxBrightness: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("maxBrightness").value = br.toString();
}

function handleStepBrightness (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 30
  }
  var obj = { kleurmode: "stepBrightness", stepBrightness: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("stepBrightness").value = br.toString();
}

function handleStartBrightness (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 60
  }
  var obj = { kleurmode: "startBrightness", startBrightness: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("startBrightness").value = br.toString();
}

function handleRood (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "rood", rood: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurRood").value = br.toString();
}

function handleGroen (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "groen", groen: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurGroen").value = br.toString();
}

function handleBlauw (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "blauw", blauw: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurBlauw").value = br.toString();
}

function handleSRood (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "srood", rood: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurSRood").value = br.toString();
}

function handleSGroen (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "sgroen", groen: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurSGroen").value = br.toString();
}

function handleSBlauw (element) {
  var br = element.value; 
  console.log(br);
  br = parseInt(br);
  if (br >= 0 && br <= 255) {
  } else {
    br = 255
  }
  var obj = { kleurmode: "sblauw", blauw: br.toString() };
  var myJSON = JSON.stringify(obj);
  console.log(myJSON);
  websocket.send(myJSON);
  document.getElementById("kleurSBlauw").value = br.toString();
}
