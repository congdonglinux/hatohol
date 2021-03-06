/*
 * Copyright (C) 2015 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License, version 3
 * as published by the Free Software Foundation.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Hatohol. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var SeverityRanksView = function(userProfile) {
  //
  // Variables
  //
  var self = this;
  var rawData;

  // call the constructor of the super class
  HatoholMonitoringView.apply(this, [userProfile]);

  //
  // main code
  //
  load();

  //
  // Main view
  //
  var severityChoices = [
    gettext("Not classified"),
    gettext("Information"),
    gettext("Warning"),
    gettext("Average"),
    gettext("High"),
    gettext("Disaster")
  ];

  function drawTableBody(replyData) {
    var html, severityRank, severityRankId, status, color, label, asImportant;
    var defaultLabel;
    html = "";

    for (var x = 0; x < replyData["SeverityRanks"].length; ++x) {
      severityRank = replyData["SeverityRanks"][x];
      severityRankId = severityRank["id"];
      status = severityRank["status"];
      color = severityRank["color"];
      label = severityRank["label"];
      asImportant = severityRank["asImportant"];
      defaultLabel = severityChoices[status];

      html += "<tr>";
      html += "<td id='severity-rank-status" + escapeHTML(status) +"'>" +
        severityChoices[Number(status)] + "</td>";
      html += "<td id='severity-rank-color" + escapeHTML(status) + "'" +
        " style='background-color: " + escapeHTML(color) + "'>" +
        escapeHTML(color) + "</td>";
      html += "<td><input type=\"text\" id='severity-rank-label" + escapeHTML(status) +"'" +
        "contenteditable='true' placeholder='" + defaultLabel + "' value='" +
	 escapeHTML(label) + "'></td>";
      html += "<td class='delete-selector'>";
      html += "<input type='checkbox' id='severity-rank-checkbox" +
        escapeHTML(status) +"'";
      if (asImportant) {
        html += "checked='checked'";
      }
      html += " class='selectcheckbox' " +
        "severityRankStatus='" + escapeHTML(status) + "'></td>";

      html += "</tr>";
    }

    return html;
  }

  function makeQueryData(status) {
    var queryData = {};
    queryData.status = status;
    queryData.color = $("#severity-rank-color" + status).text();
    queryData.label = $("#severity-rank-label" + status).val();
    queryData.asImportant = $("#severity-rank-checkbox" + status).is(":checked");
    return queryData;
  }

  function saveSeverityRank(status) {
    var deferred = new $.Deferred();
    var url = "/severity-rank";
    url += "/" + status;
    new HatoholConnector({
      url: url,
      request: "POST",
      data: makeQueryData(status),
      replyCallback: function() {
        deferred.resolve();
      },
      parseErrorCallback: function() {
        deferred.reject();
      },
      connectErrorCallback: function() {
        deferred.reject();
      },
    });
    return deferred.promise();
  }

  function saveSeverityRanks() {
    var severityRanks = rawData["SeverityRanks"];
    var promise, promises = [];

    // TODO: Save only modified ranks

    $.map(severityRanks, function(rank, i) {
      promise = saveSeverityRank(rank.status);
      promises.push(promise);
    });

    hatoholInfoMsgBox(gettext("Saving..."));

    $.when.apply($, promises)
      .done(function() {
        hatoholInfoMsgBox(gettext("Succeeded to save."));
        self.startConnection('severity-rank', updateCore);
      })
      .fail(function() {
        hatoholInfoMsgBox(gettext("Failed to save!"));
        self.startConnection('severity-rank', updateCore);
      });
  }

  function setupApplyButton(reply) {
    if (!userProfile.hasFlag(hatohol.OPPRVLG_UPDATE_SEVERITY_RANK))
      return;

    $("#save-severity-ranks").show();
    $("#save-severity-ranks").click(function() {
      saveSeverityRanks();
    });
    $("#cancel-severity-ranks").show();
    $("#cancel-severity-ranks").click(function() {
      load();
      hatoholInfoMsgBox(gettext("Input data has reset."));
    });
  }

  function setupColorPickers(reply) {
    var i, color, severityRanks = reply["SeverityRanks"];
    for (i = 0; i < severityRanks.length; ++i) {
      var colorSelector = "#severity-rank-color" + severityRanks[i].status;
      color = severityRanks[i].color;
      setupSpectrum(color, colorSelector);
    }

    function setupSpectrum(color, colorSelector) {
      $(colorSelector).spectrum({
        color: color,
        change: function(color) {
          var colorHexString = color.toHexString();
          $(colorSelector).text(colorHexString);
          $(colorSelector).css("background-color", colorHexString);
        }
      });
    }
  }

  function drawTableContents(data) {
    $("#table tbody").empty();
    $("#table tbody").append(drawTableBody(data));
  }

  function getQuery() {
    return 'severity-rank';
  }

  function updateCore(reply) {
    rawData = reply;

    drawTableContents(rawData);
    setupApplyButton(rawData);
    setupColorPickers(rawData);
  }

  function load() {
    self.displayUpdateTime();

    self.startConnection(getQuery(), updateCore);
  }
};

SeverityRanksView.prototype = Object.create(HatoholMonitoringView.prototype);
SeverityRanksView.prototype.constructor = SeverityRanksView;
