const Modes = { GAS: 1, WOOD: 2, WARMY: 4, COLDY: 8}
$( document ).ready(function() {
	
	(function worker() {
		$.getJSON('/state', function(data) {
			document.getElementById('counter').textContent = data.counter;
			document.getElementById('date_time').textContent = data.date_time;
			
			document.getElementById('tank_temp').innerHTML = data.tank_temp + ' &deg;C';
			var tank_tempPanel = document.getElementById('tank_temp-panel');
			if (data.tank_healthy) {
				tank_tempPanel.classList.remove("panel-danger");
				tank_tempPanel.classList.add("panel-default");
			}
			else {
				tank_tempPanel.classList.remove("panel-default");
				tank_tempPanel.classList.add("panel-danger");
			}
			
			document.getElementById('valve_temp').innerHTML = data.valve_temp + ' &deg;C';
			var valve_tempPanel = document.getElementById('valve_temp-panel');
			if (data.valve_healthy) {
				valve_tempPanel.classList.remove("panel-danger");
				valve_tempPanel.classList.add("panel-default");
			}
			else {
				valve_tempPanel.classList.remove("panel-default");
				valve_tempPanel.classList.add("panel-danger");
			}
			
			var modePanel = document.getElementById('mode');
			if (data.mode & Modes.WOOD) {modePanel.textContent = "Wood";}
			if (data.mode & Modes.WARMY) {modePanel.textContent = "Gas";} //GAS by now, will change later to Warmy again
			if (data.mode & Modes.COLDY) {modePanel.textContent = "Coldy";}
			setTimeout(worker, 5000);
		});
	})();
});