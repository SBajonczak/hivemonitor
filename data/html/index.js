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
    document.getElementById("cv").hidden = true;
}

function toogleAutoTare() {
    ActualPage = "TARE";
    var ct = document.getElementById("ct");

}

function sendData(data) {
    var req = new XMLHttpRequest();
    req.addEventListener('load', function () { showToastMessage(this.responseText) });
    req.addEventListener('error', function (e) { showToastMessage(e.stack, true); });
    req.open("POST", "/upload");
    var formData = new FormData();
    formData.append("data", fO.files[0], name);
    req.send(formData);
    fO.value = '';
    return false;
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