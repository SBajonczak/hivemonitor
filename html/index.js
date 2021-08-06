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
    setInitialPage();
    setDisplay("cv", "none");
    GetSettings();

}
function GetSettings() {
    read("settings").then((data) => {
        setTextBoxValue("system_sleeptime", data.system.sleeptime);
        setTextBoxValue("system_vccadjustsetting", data.system.vccadjustsetting);

        setTextBoxValue("mqtt_server", data.mqtt.server);
        setTextBoxValue("mqtt_user", data.mqtt.user);
        setTextBoxValue("mqtt_password", data.mqtt.password);
        setTextBoxValue("mqtt_port", data.mqtt.port);

        setTextBoxValue("scale_weightoffset", data.scale.weightoffset);
        setTextBoxValue("scale_kilogramdivider", data.scale.kilogramdivider);
        setTextBoxValue("scale_calibrationtemperaturesetting", data.scale.calibrationtemperaturesetting);
        setTextBoxValue("scale_calibrationfactorsetting", data.scale.calibrationfactorSetting);

        setTextBoxValue("wireles_password", data.wireles.password);
        setTextBoxValue("wireles_ssid", data.wireles.ssid);




    });
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
    setDisplay("cSettings", "block");

}

function hideAllControls() {
    setDisplay("cTareStep0", "none");
    setDisplay("cTareStep1", "none");
    setDisplay("cTareStep2", "none");
    setDisplay("cSettings", "none");
    setDisplay("welcome", "none");
    setDisplay("cv", "none");
}

function setDisplay(control, state) {

    var ct = document.getElementById(control);
    if (state === "block") {
        ct.style.display = "";
    } else {
        ct.style.display = state;
    }
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
    }).catch(() => { });
}

function startTare() {
    sendData({}, "tarestep0").then((data) => {
        ActualPage = "TARE1";
        setControls();
    }).catch(() => { });
}

function sendData(json, uri) {
    var prom = new Promise((resolve, reject) => {
        var req = new XMLHttpRequest();
        req.addEventListener('load', function () { showToastMessage("Settings saved!"); resolve(this.responseText); });
        req.addEventListener('error', function (e) { showToastMessage("Error while saving Settings!", true); reject(); });
        req.open("POST", uri);
        req.setRequestHeader('Content-Type', 'application/json');
        req.send(JSON.stringify(json));
    })
    return prom;
}


function read(uri) {
    var prom = new Promise((resolve, reject) => {
        var req = new XMLHttpRequest();
        req.addEventListener('error', function (e) { showToastMessage("Error while getting Data ", true); reject(); });
        req.open("GET", uri);
        req.onload = function (data) {
            resolve(data);
        }
    })
    return prom;
}


var timeout;

function showToastMessage(text, error = true) {
    var x = d.getElementById(error ? "toastErrorText" : "toastSuccessText");
    var container = d.getElementById(error ? "toastError" : "toastSuccess");

    x.innerText = text;
    clearTimeout(timeout);

    container.style.display = 'block';
    timeout = setTimeout(function () { container.style.display = "hidden"; }, 2900);
}

function getTextBoxValue(id) {
    return d.getElementById(id).value;
}


function getTextBoxNumberValue(id) {
    var n = d.getElementById(id).value;
    return parseFloat(n);
}


function setTextBoxValue(id, value) {
    d.getElementById(id).value = value;
}

function saveSettings() {
    var objectData = {
        system: {
            sleeptime: getTextBoxNumberValue('system_sleeptime'),
            vccadjustsetting: getTextBoxNumberValue('system_vccadjustsetting')
        },
        mqtt: {
            server: getTextBoxValue('mqtt_server'),
            port: getTextBoxNumberValue('mqtt_port'),
            user: getTextBoxValue('mqtt_user'),
            password: getTextBoxValue('mqtt_password')
        },
        scale: {
            weightoffset: getTextBoxNumberValue('scale_weightoffset'),
            kilogramdivider: getTextBoxNumberValue('scale_kilogramdivider'),
            calibrationtemperaturesetting: getTextBoxNumberValue('scale_calibrationtemperaturesetting'),
            calibrationfactorSetting: getTextBoxNumberValue('scale_calibrationfactorsetting')
        },
        wireles: {
            password: getTextBoxValue('wireles_password'),
            ssid: getTextBoxValue('wireles_ssid')
        }
    }
    sendData(objectData, "store").then((data) => {
        ActualPage = "TARE2";
        setControls();
    }).catch(() => { });
}