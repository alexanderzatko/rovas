---
title: Glosář
format: filtered_html
---

<p><strong><a name="neo">NEO</a></strong></p>

<p>NEO is an economic system where any actually performed activity can be rewarded, if the product of that activity can be consumed freely, or by paying the NEO currency <a href="#chron">chron</a>. NEO workers are rewarded with a flat monetary reward of 10 chrons for every hour of their work and if somebody pays for the product or service produced by that work, they receive also <a href="#merit">merit</a> reward. To earm chrons, users must file a<a href="#workReport"> work report</a> in the NEO application <a href="#rovas">Rovas</a>, which issues a chron reward, once the report gets <a href="#verification">verified</a> by two or more NEO workers. All transactions and user chron and merit account movements and balances are publicly visible to enable public control. Any NEO user can open a dispute against another user if they suspect they violated the NEO rules.</p>

<p><strong><a name="rovas">Rovas</a></strong></p>

<p>Rovas is a web application implementing the NEO rules. Anybody can register and use the application by visiting its internet address at https://rovas.app or by utilizing the <a href="https://neofund.sk/rovas-api">Rovas API</a>.</p>

<p><strong><a name="project">Project</a></strong></p>

<p>To earn merit reward, the good or service producer must create a project in Rovas. The buyers of the products or services then are able to pay for their consumption by clicking the "Purchase/Reward" button on the project page. The project owner can specify how merits gained from sales of the product are divided among the individual or other projects participants who helped to create the good or service produced by assigning them <a href="#shareholder">shares</a> in the project. The shares can be set by percentage, or by labor time. There are two main types of projects - <strong>private</strong>, where the project owner invites workers and allocates them the shares, or <strong>public</strong>, which anybody can join the project and where the shares are allocated purely by the labor time reported by the participants. In order to earn a chron reward for working on a particular project (and earn a merit share for their contribution), worker must file a <a href="#workReport">work report</a> in that project.</p>

<p><strong><a name="systemProject">System project</a></strong></p>

<p>Upon registration, Rovas creates a special project for the registering user to make it easier for them to earn a reward for creating their Rovas account. This project should not be used for other types of work reports</p>

<p><strong><a name="shareholder">Shareholder</a></strong></p>

<p>Every Rovas <a href="#project">project</a> has one or more shareholders. These are users, who have right to post work reports in the project and receive a share of Merits granted to the project, when somebody buys or rewards the product of the project. The share of Merits a shareholder receives can be based on the length of the labor time they invested into the project, a percentual amount, or a combination of the two methods. The particular allocation of merits for a given project determines the project owner in the project form.</p>

<p><strong><a name="workReport">Work report</a></strong></p>

<p>Work report is a record of labor performed by a worker on a particular <a href="#project">project</a>. Work reports contain the date when the work started, the labor time length, the NACE category for the performed work and one or more types of proofs that evidence the work was actually performed. The <a href="/rules">NEO rules</a> dictate that only work reports that have been provably performed and were filed after the worker registered in Rovas can earn chrons. Notably, immadiate usefulness of the performed work is <strong>not</strong> a condition for earning the chron reward, reflecting the fact that many types of products or services gain social value only after often a long time.</p>

<p><strong><a name="verification">Verification</a></strong></p>

<p>To ensure stability of the NEO economy, every work report filed must be approved by two other users, to qualify for receiving the chron reward. The verifier selection is made by a Rovas algorithm, selecting the most suitable candidates by their field of expertise as indicated by the <a href="#nace">NACE</a> codes and by the users' merit scores. The verifiers are asked to verify a given report via email, es well as by visibly placed notice in Rovas.</p>

<p><strong><a name="chron">Chron</a></strong></p>

<p>Chron is the <strong>currency</strong> of the <a href="#neo">economic system NEO</a>. Anybody can earn chrons for work they like to do, by filing a work report in <a href="#rovas">Rovas</a>. After the two Rovas-selected users judge that the reported time is realistic, Rovas grants the worker who filled the report 10 chrons for every reported hour. Other conditions necessary for earning chron reward are listed on the <a href="/rules">Rules page</a>.</p>

<p><strong><a name="merit">Merit</a></strong></p>

<p>Merit is non-monetary, non-transferable, reputation-like <strong>measure of social value</strong> of one's work, granted to producers of goods or services, when somebody buys, or rewards them with their own chrons. The chrons are paid not to the producer of the purchased good or services but to Rovas, which converts them into merits - one chron for one merit (the chrons paid are destroyed). The user's merit score is a quantity reflecting the multidimensionality of human abilities and skills, as it consists of rewards for individual NACE categories, listed in his work reports. If an user's merit score becomes negative - for example when their Compliance score drops below 0 - the user looses ability to post work reports and earn chrons. However, they can continue to create new projects and earn merits. Once their merit score becomes positive, they regain access to all Rovas services.</p>

<p><strong><a name="complianceScore">Compliance score</a></strong></p>

<p>In NEO, every user is assigned a numeric score that reflectes the level of theor conformity with the NEO rules. After creating an account in Rovas the score is 0 and is incremented by 1 when:</p>

<ul>
	<li>the user <a href="#verification">verifies</a> some other user's report</li>
</ul>

<p>The score is decreased by 1 when:</p>

<ul>
	<li>the user's work report is rejected</li>
	<li>the user fails to verify a work report</li>
</ul>

<p>Once user's compliance score reaches value -3, Rovas subtracts 10 merits form the user's score and resets the compliance score back to 0.</p>

<p><strong><a name="nace">NACE</a></strong></p>

<p>Every work report must contain information about the type of work performed. NEO uses the <a href="https://en.wikipedia.org/wiki/Statistical_Classification_of_Economic_Activities_in_the_European_Community">NACE</a> work types classification scheme. A proper NACE class is aassigned to a work by the user when filing a work report. The main reason why reports must have this information is to aid the verification process. Rovas uses the NACE code to allocate a given work report for verification to the users, who have filed work reports with the same or similar NACE classification, on order to ensure the verification process is efficient. Assigning NACE information to work reports also makes finding workers with the desired type and level of expertise easy.</p>

<p><strong><a name="dispute">Dispute</a></strong></p>

<p>Dispute is a procedure, where one NEO user can accuse another for violating the NEO rules. The violation might concern a work report filed for work that was not performed, or one that generates product or service impossible to freely consume or buy with chrons. After filing a dispute, both parties solicit support from other users who vote for their candidate. After a set period of time, the cummulative merit scores of the supporters of both parties are tallied and the two numbers compared. The party with more merits wins. A more detailed description of the process can be found on the <a href="/disputes">Disputes page</a>.</p>

<p><strong><a id="meritware" name="meritware">Meritware</a></strong></p>

<p>The owner of a project is free to determine how rewards granted to their project will be handled. The default setting for a newly created project is to have 100% of a reward converted to merits, which makes such a project <strong>meritware</strong>. The concept exists to provide a clear signal that any potential reward to the project will be converted 100% into merits and therefore will not monetarily enrich the project owner.</p>

<p><a id="projectMEalloc" name="projectMEalloc"><strong>Reward merit allocation</strong></a></p>

<p>A <a href="#project">project</a> owner can specify how the rewards paid to the project will be allocated. Using the Reward merit allocation field of the project form, they can either leave it blank, or set a number between 0 and 100. The number indicates the part of the amount paid to the project (in percent), which will be converted to <a href="#merit">merits</a>. The remaining part of the payment is sent in <a href="#chron">chrons,</a> to the account of the owner, or to the beneficiary of the project. An empty value sets the merit part of the reward to 100%.</p>
