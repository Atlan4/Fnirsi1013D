// This file is used by the Search Focus Dropdown mod package.

var searchForm = document.getElementById("search_form");
var focusSelect = document.getElementById("search_focus");

searchForm.onsubmit = function()
{
	var focusChoice = focusSelect.options[focusSelect.selectedIndex].value;

	cleanSearchForm();

	if (focusChoice == "topic")
		addToSearchForm("topic", curTopic);
	else if (focusChoice == "board")
		addToSearchForm("brd["+curBoard+"]", curBoard);
	// one of the special boards selected?
	else if (!isNaN(parseFloat(focusChoice)) && isFinite(focusChoice))
		addToSearchForm("brd["+focusChoice+"]", focusChoice);

	return true;
};

function cleanSearchForm()
{
	removeFromSearchForm("topic");
	removeFromSearchForm("brd["+curBoard+"]");
	for(var i in focusBoards)
		removeFromSearchForm("brd["+focusBoards[i]+"]");
}

function removeFromSearchForm(elementName)
{
	var element = searchForm.elements[elementName];
	if (element)
		searchForm.removeChild(element);
}

function addToSearchForm(elementName, elementValue)
{
	var element = searchForm.elements[elementName];
	if (!element)
	{     
		element = document.createElement("input");
		element.setAttribute("type", "hidden");
		element.setAttribute("name", elementName);
		element.setAttribute("value", elementValue);
		searchForm.appendChild(element);
	}
}