var ActualPage = "INDEX";
var ScaleConnected = false;
var d = document;
function onLoad() {
    hideAllControls();
    parseQuery();
    setBreadCrumb();
    setInitialPage();
    setDisplay("cv", "none");
    GetSettings();

}
function disableScaleSettings() {
    console.log("disabling scale controls");
    setDisplay("scaleNotConnected", "block");
    setDisplay("btnStartTare", "none");
    setControlState("scale_calibrationfactorsetting", true);
    
    setControlState("scale_calibrationtemperaturesetting", true);
    setControlState("scale_kilogramdivider", true);
    setControlState("scale_weightoffset", true);
}
function GetSettings() {
    read("info").then((data) => {
        console.log(data);
        ScaleConnected = data.sensors.scale.connected;
        setDisplay("scaleNotConnected", "none");
        if (!ScaleConnected) {
            disableScaleSettings();
        } else {
            console.log("Scale connected");
        }
    });
    read("settings").then((data) => {
        console.debug(data);
        setTextBoxValue("system_sleeptime", data.system.sleeptime);
        setTextBoxValue("system_vccadjustsetting", data.system.vccadjustsetting);

       
        setTextBoxValue("scale_weightoffset", data.scale.weightoffset);
        setTextBoxValue("scale_kilogramdivider", data.scale.kilogramdivider);
        setTextBoxValue("scale_calibrationtemperaturesetting", data.scale.calibrationtemperaturesetting);
        setTextBoxValue("scale_calibrationfactorsetting", data.scale.calibrationfactorsetting);

        setTextBoxValue("wireles_password", data.wireles.password);
        setTextBoxValue("wireles_ssid", data.wireles.ssid);
    });
}
function setInitialPage() {
    switch (ActualPage) {
        case "INDEX":
            toggleIndex();
            break;
        case "TARE_START":
            toogleAutoTare();
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

function setStyle(control, style) {
    var ct = document.getElementById(control);
    ct.style = style;
}

function setControlState(control, disabled) {
    var ct = document.getElementById(control);
    if (disabled) {
        ct.disabled = "disabled";
    } else {
        ct.disabled = "";

    }
}



function toogleAutoTare() {
    ActualPage = "TARE_START";
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
        case "TARE_FINISH":
            pl.innerText = "Tare Scale";
            break;
    }


}

function startTare() {

    setLabel("measuringMessage", "Measuring! Please Wait... ");
    setDisplay("measuring", "block");
    console.log("Actual Page:" + ActualPage);
    setDisplay("btnTare", "none");
    switch (ActualPage) {
        case "TARE_STEP2":
            read("getWeightValue").then((data) => {
                setDisplay("btnTare", "block");
                setTextBoxValue("scale_kilogramdivider", data);
                ActualPage = "INDEX";
                setControls();
            }).catch(() => { });
            break;
        case "TARE_START":
            read("getWeightValue").then((data) => {
                setDisplay("btnTare", "block");
                setDisplay("measuring", "none");
                setDisplay("tareStep2", "block")
                setLabel("measuringMessage", "".concat("The measured weight offset is: ", data));
                setTextBoxValue("scale_weightoffset", data);
                ActualPage = "TARE_STEP2";
                setControls();
                setDisplay("measuring", "block");
                setDisplay("cTareStep0", "block");
            }).catch(() => { });
            break;
        default:

    }

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
            resolve(JSON.parse(req.responseText));
        }
        req.send();
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


function setLabel(id, value) {
    d.getElementById(id).innerHTML = value;
}


function saveSettings() {
    var objectData = {
        system: {
            sleeptime: getTextBoxNumberValue('system_sleeptime'),
            vccadjustsetting: getTextBoxNumberValue('system_vccadjustsetting')
        },
        // mqtt: {
        //     server: getTextBoxValue('mqtt_server'),
        //     port: getTextBoxNumberValue('mqtt_port'),
        //     user: getTextBoxValue('mqtt_user'),
        //     password: getTextBoxValue('mqtt_password')
        // },
        scale: {
            weightoffset: getTextBoxNumberValue('scale_weightoffset'),
            kilogramdivider: getTextBoxNumberValue('scale_kilogramdivider'),
            calibrationtemperaturesetting: getTextBoxNumberValue('scale_calibrationtemperaturesetting'),
            calibrationfactorsetting: getTextBoxNumberValue('scale_calibrationfactorsetting')
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