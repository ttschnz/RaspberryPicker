const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <title>System Status Dashboard</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0" />
      <style>
        body {
          font-family: Arial, sans-serif;
          background-color: #f4f6f8;
          margin: 0;
          padding: 20px;
        }

        h1 {
          text-align: center;
          margin-bottom: 30px;
          color: #333;
        }

        table {
          width: 60%;
          margin: 0 auto;
          border-collapse: collapse;
          background-color: #fff;
          box-shadow: 0 2px 8px rgba(0,0,0,0.1);
          border-radius: 8px;
          overflow: hidden;
        }

        @media only screen and (max-width: 800px) {
          table {
            width: 90vw;
          }
        }

        colgroup col:first-child {
          width: 25%; /* 1 part */
        }

        colgroup col:last-child {
          width: 75%; /* 3 parts */
        }

        th, td {
          padding: 12px 15px;
          text-align: left;
          border-bottom: 1px solid #ddd;
        }

        th {
          background-color: #2c3e50;
          color: #fff;
          text-transform: uppercase;
          font-size: 0.9rem;
          letter-spacing: 0.05em;
        }

        tr:hover {
          background-color: #f1f1f1;
        }

        .value {
          font-weight: bold;
          color: #2c3e50;
        }
      </style>
    </head>
    <body>

      <h1>Raspberry Picker Interface</h1>

      <table>
        <colgroup>
          <col>
          <col>
        </colgroup>
        <thead>
          <tr>
            <th>Parameter</th>
            <th>Value</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Door State</td>
            <td id="door-state" class="value"></td>
          </tr>
          <tr>
            <td>Door Position</td>
            <td id="door-position" class="value"></td>
          </tr>
          <tr>
            <td>Sorting State</td>
            <td id="sorting-state" class="value"></td>
          </tr>
          <tr>
            <td>Sorting Position</td>
            <td id="sorting-position" class="value"></td>
          </tr>
          <tr>
            <td>Fill Count (Small)</td>
            <td id="fill-count-small" class="value"></td>
          </tr>
          <tr>
            <td>Fill Count (Large)</td>
            <td id="fill-count-large" class="value"></td>
          </tr>
        </tbody>
      </table>

      <script>
        setInterval(async () => {
          try {
            const response = await fetch("/api/state");
            const data = await response.json();

            document.getElementById('door-state').textContent = data["door.state"];
            document.getElementById('door-position').textContent = data["door.position"];
            document.getElementById('sorting-state').textContent = data["sorting.state"];
            document.getElementById('sorting-position').textContent = data["sorting.position"];
            document.getElementById('fill-count-small').textContent = data["fill_count.small"];
            document.getElementById('fill-count-large').textContent = data["fill_count.large"];
          } catch (error) {
            console.error("Error fetching state:", error);
          }
        }, 100);
      </script>

    </body>
    </html>

)rawliteral";
