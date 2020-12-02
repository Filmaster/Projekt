function showTime() {
    var date = new Date();
    var h = date.getHours();
    var m = date.getMinutes();
    var s = date.getSeconds();
    var session = "AM";

    if (h == 0) {
        h = 12;
    }

    if (h > 12) {
        h = h - 12;
        session = "PM";
    }

    h = (h < 10) ? "0" + h : h;
    m = (m < 10) ? "0" + m : m;
    s = (s < 10) ? "0" + s : s;

    var time = h + ":" + m + ":" + s + " " + session;
    document.getElementById("aktualniCas").innerText = time;
    document.getElementById("aktualniCas").textContent = time;

    setTimeout(showTime, 1000);

}

let casy = [];
const pridejCas = (ev) => {
    ev.preventDefault();  //to stop the form submitting
    let cas = {
        cas1: document.getElementById('cas1').value,
        cas2: document.getElementById('cas2').value
    }
    casy.push(cas);
    //document.querySelector('form').reset();

    //for display purposes only
    console.warn('added', { casy });

    //saving to localStorage
    localStorage.setItem('data', JSON.stringify(casy));
}

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('btn').addEventListener('click', pridejCas);
});


showTime();