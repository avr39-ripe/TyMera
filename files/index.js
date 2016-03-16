const Modes = { GAS: 1, WOOD: 2, WARMY: 4, COLDY: 8}
$( document ).ready(function() {
	
	(function worker() {
		$.getJSON('/state', function(data) {
			document.getElementById('counter').textContent = data.counter;
			document.getElementById('temperature').innerHTML = data.temperature + ' &deg;C';
			var tempPanel = document.getElementById('temperature-panel');
			if (data.healthy) {
				tempPanel.classList.remove("panel-danger");
				tempPanel.classList.add("panel-default");
			}
			else {
				tempPanel.classList.remove("panel-default");
				tempPanel.classList.add("panel-danger");
			}
			var modePanel = document.getElementById('mode');
			if (data.mode & Modes.WOOD) {modePanel.textContent = "Wood";}
			if (data.mode & Modes.WARMY) {modePanel.textContent = "Gas";} //GAS by now, will change later to Warmy again
			if (data.mode & Modes.COLDY) {modePanel.textContent = "Coldy";}
			setTimeout(worker, 5000);
		});
	})();
});