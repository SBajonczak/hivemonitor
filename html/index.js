var ActualPage = "INDEX";

var cfg = {
    theme: { base: "dark", bg: { url: "" }, alpha: { bg: 0.6, tab: 0.8 }, color: { bg: "" } },
    comp: {
        colors: { picker: true, rgb: false, quick: true, hex: false },
        labels: true, pcmbot: false, pid: true, seglen: false, css: true, hdays: false
    }
};


var d = document;
function onLoad() {
    hideAllControls();
    parseQuery();
    setBreadCrumb();
    document.getElementById("cv").hidden = true;
    setInitialPage();
}


function setInitialPage() {
    switch (ActualPage) {
        case "INDEX":
            toggleIndex();
            break;
        case "TARE":
            toogleAutoTare();
            break;
        case "TARE1":
            setDisplay("cTareStep1", "block");
            break;
        case "TARE2":
            setDisplay("cTareStep2", "block");
            break;
    }
}
function parseQuery() {
    var param = new URLSearchParams(document.location.search);
    ActualPage = param.get('page');
    if (ActualPage == null) {
        ActualPage = "INDEX";
    }
}

function toggleIndex() {
    hideAllControls();
    setDisplay("welcome", "block");
    setDisplay("btnbar", "block");


}

function hideAllControls() {
    setDisplay("cTareStep0", "none");
    setDisplay("cTareStep1", "none");
    setDisplay("cTareStep2", "none");
    setDisplay("welcome", "none");
    setDisplay("btnbar", "none");




}
function setDisplay(control, state) {
    var ct = document.getElementById(control);
    ct.style.display = state;
}
function toogleAutoTare() {
    ActualPage = "TARE";
    hideAllControls();
    setDisplay("cTareStep0", "block");
}

function setControls() {
    hideAllControls();
    setBreadCrumb();
    setInitialPage();

}
function setBreadCrumb() {
    var pl = document.getElementById("pl");
    switch (ActualPage) {
        case "INDEX":
            pl.innerText = "Start";
            break;
        case "TARE":
        case "TARE1":
        case "TARE2":
            pl.innerText = "Tare Scale";
            break;
    }


}
function tareStep2() {
    sendData({}, "tarestep1").then((data) => {
        ActualPage = "TARE2";
        setControls();
    }).catch(()=>{});
}

function startTare() {
    sendData({}, "tarestep0").then((data) => {
        ActualPage = "TARE1";
        setControls();
    }).catch(()=>{});
}

function sendData(json, uri) {
    var prom = new Promise((resolve, reject) => {
        var req = new XMLHttpRequest();
        req.addEventListener('load', function () { showToastMessage("OK!"); resolve(this.responseText); });
        req.addEventListener('error', function (e) { showToastMessage(e.stack, true); reject(); });
        req.open("POST", uri);
        req.setRequestHeader('Content-Type', 'application/json');
        req.send(JSON.stringify(json));
    })
    return prom;
}


var timeout;
function showToastMessage(text, error = false) {
    if (error) d.getElementById('connind').style.backgroundColor = "#831";
    var x = d.getElementById("toast");
    x.innerHTML = text;
    x.className = error ? "error" : "show";
    clearTimeout(timeout);
    x.style.animation = 'none';
    x.style.animation = null;
    timeout = setTimeout(function () { x.className = x.className.replace("show", ""); }, 2900);
}