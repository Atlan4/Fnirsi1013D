/**
 * Say Thanks
 *
 * @package SMF
 * @author kelvincool
 * @copyright 2015 kelvincool
 * @license http://creativecommons.org/licenses/by/3.0 CC
 *
 * @version 1.3
 */

function saythanks() {
	this.init();
}

/**
 *	Initialize variables and listeners.
 */
saythanks.prototype.init = function() {
	if (typeof(document.getElementsByClassName) == "function") {
		var oThanksButtons = document.getElementsByClassName('thank_you_button_link');
		var oWithdrawThanksButtons = document.getElementsByClassName('withdraw_thank_you_button_link');
	}
	else {
		var oThanksButtons = document.querySelectorAll('.thank_you_button_link');
		var oWithdrawThanksButtons = document.querySelectorAll('.withdraw_thank_you_button_link');
	}
	for (i = 0; i < oThanksButtons.length; i++) {
		var oThanksButton = oThanksButtons[i];
		oThanksButton.instanceRef = this;
		oThanksButton.onclick = function () {
			this.instanceRef.handleThankClick(this);
			return false;
		};
	}
	for (i = 0; i < oWithdrawThanksButtons.length; i++) {
		var oThanksButton = oWithdrawThanksButtons[i];
		oThanksButton.instanceRef = this;
		oThanksButton.onclick = function () {
			this.instanceRef.handleThankClick(this);
			return false;
		};
	}
}

/**
 *	Handle thanks click
 */
saythanks.prototype.handleThankClick = function (oInput)
{
	var sThanksLoading = document.getElementById('thanks_loading').innerHTML;
	var oLi = oInput.parentNode;
	oLi.className = 'thank_you_loading';
	oLi.innerHTML = sThanksLoading;
	var xhr = typeof XMLHttpRequest != 'undefined' ? new XMLHttpRequest() : new ActiveXObject('Microsoft.XMLHTTP');
	xhr.onreadystatechange = function() {
		var DONE = this.DONE || 4;
		if (xhr.readyState == DONE) {
			if (xhr.status == 200) {
				data = JSON.parse(xhr.responseText);
				oLi.className = 'thank_you_' + data.result;
				oLi.innerHTML = data.response;
				if (data.thanker == "") {
					var elem = document.getElementById('thanker_' + data.id + '_' + data.member);
					if (elem.parentElement.getElementsByTagName('a').length == 1) {
						elem.parentElement.parentElement.removeChild(elem.parentElement);
					}
					else {
						var prev = elem.previousSibling;
						if (prev != null && prev.className == 'thanker_separator') {
							prev.parentElement.removeChild(prev);
						}
						var next = elem.nextSibling;
						if (next != null && next.className == 'thanker_separator') {
							next.parentElement.removeChild(next);
						}
						elem.parentElement.removeChild(elem);
					}
				}
				else if (data.thanker != "") {
					thankerList = document.getElementById('thanker_list_' + data.id);
					if (thankerList == null) {
						thankerList = document.getElementById('thank_you_placeholder_' + data.id).parentElement;
						if (thankerList != null) {
							thankerList.innerHTML += data.list;
						}
					}
					else {
						thankerList.innerHTML += data.thanker;
					}
				}
				if (data.refresh != null) {
					msgPost = document.getElementById('msg_' + data.id);
					msgPost.innerHTML = data.refresh;
				}
				saythanks.prototype.init();
			}
			else {
				var sThanksError = document.getElementById('thanks_error').innerHTML;
				oLi.className = 'thank_you_error';
				oLi.innerHTML = sThanksError;
			}
		}
	}

	xhr.open('GET', oInput.href + ';ajax=1;xml=1', true);
	xhr.setRequestHeader('X-Requested-With', 'XMLHttpRequest');
	xhr.send(null);
}
