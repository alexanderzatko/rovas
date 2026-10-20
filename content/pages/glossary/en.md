---
title: Glossary
format: filtered_html
---

**<a name="neo"></a>NEO**

NEO is an economic system where any actually performed activity can be rewarded, if the product of that activity can be consumed freely, or by paying the NEO currency [chron](#chron). NEO workers are rewarded with a flat monetary reward of 10 chrons for every hour of their work and if somebody pays for the product or service produced by that work, they receive also [merit](#merit) reward. To earm chrons, users must file a [work report](#workReport) in the NEO application [Rovas](#rovas), which issues a chron reward, once the report gets [verified](#verification) by two or more NEO workers. All transactions and user chron and merit account movements and balances are publicly visible to enable public control. Any NEO user can open a dispute against another user if they suspect they violated the NEO rules.

**<a name="rovas"></a>Rovas**

Rovas is a web application implementing the NEO rules. Anybody can register and use the application by visiting its internet address at https://rovas.app or by utilizing the [Rovas API](https://neofund.sk/rovas-api).

**<a name="project"></a>Project**

To earn merit reward, the good or service producer must create a project in Rovas. The buyers of the products or services then are able to pay for their consumption by clicking the "Purchase/Reward" button on the project page. The project owner can specify how merits gained from sales of the product are divided among the individual or other projects participants who helped to create the good or service produced by assigning them [shares](#shareholder) in the project. The shares can be set by percentage, or by labor time. There are two main types of projects - **private**, where the project owner invites workers and allocates them the shares, or **public**, which anybody can join the project and where the shares are allocated purely by the labor time reported by the participants. In order to earn a chron reward for working on a particular project (and earn a merit share for their contribution), worker must file a [work report](#workReport) in that project.

**<a name="systemProject"></a>System project**

Upon registration, Rovas creates a special project for the registering user to make it easier for them to earn a reward for creating their Rovas account. This project should not be used for other types of work reports

**<a name="shareholder"></a>Shareholder**

Every Rovas [project](#project) has one or more shareholders. These are users, who have right to post work reports in the project and receive a share of Merits granted to the project, when somebody buys or rewards the product of the project. The share of Merits a shareholder receives can be based on the length of the labor time they invested into the project, a percentual amount, or a combination of the two methods. The particular allocation of merits for a given project determines the project owner in the project form.

**<a name="workReport"></a>Work report**

Work report is a record of labor performed by a worker on a particular [project](#project). Work reports contain the date when the work started, the labor time length, the NACE category for the performed work and one or more types of proofs that evidence the work was actually performed. The [NEO rules](/rules) dictate that only work reports that have been provably performed and were filed after the worker registered in Rovas can earn chrons. Notably, immadiate usefulness of the performed work is **not** a condition for earning the chron reward, reflecting the fact that many types of products or services gain social value only after often a long time.

**<a name="verification"></a>Verification**

To ensure stability of the NEO economy, every work report filed must be approved by two other users, to qualify for receiving the chron reward. The verifier selection is made by a Rovas algorithm, selecting the most suitable candidates by their field of expertise as indicated by the [NACE](#nace) codes and by the users' merit scores. The verifiers are asked to verify a given report via email, es well as by visibly placed notice in Rovas.

**<a name="chron"></a>Chron**

Chron is the **currency** of the [economic system NEO](#neo). Anybody can earn chrons for work they like to do, by filing a work report in [Rovas](#rovas). After the two Rovas-selected users judge that the reported time is realistic, Rovas grants the worker who filled the report 10 chrons for every reported hour. Other conditions necessary for earning chron reward are listed on the [Rules page](/rules).

**<a name="merit"></a>Merit**

Merit is non-monetary, non-transferable, reputation-like **measure of social value** of one's work, granted to producers of goods or services, when somebody buys, or rewards them with their own chrons. The chrons are paid not to the producer of the purchased good or services but to Rovas, which converts them into merits - one chron for one merit (the chrons paid are destroyed). The user's merit score is a quantity reflecting the multidimensionality of human abilities and skills, as it consists of rewards for individual NACE categories, listed in his work reports. If an user's merit score becomes negative - for example when their Compliance score drops below 0 - the user looses ability to post work reports and earn chrons. However, they can continue to create new projects and earn merits. Once their merit score becomes positive, they regain access to all Rovas services.

**<a name="complianceScore"></a>Compliance score**

In NEO, every user is assigned a numeric score that reflectes the level of theor conformity with the NEO rules. After creating an account in Rovas the score is 0 and is incremented by 1 when:

- the user [verifies](#verification) some other user's report

The score is decreased by 1 when:

- the user's work report is rejected
- the user fails to verify a work report

Once user's compliance score reaches value -3, Rovas subtracts 10 merits form the user's score and resets the compliance score back to 0.

**<a name="trustedUser"></a>Trusted user**

A Trusted user is a Rovas account that has earned sticky trusted status. Eligibility requires either a [Compliance score](#complianceScore) of at least 50, or owning a published [project](#project) that has received more than 500 [Merits](#merit) from rewards to that project (owning shares alone is not enough). Once granted, Trusted status stays until eligibility fails after a lost [dispute](#dispute). Trusted users may optionally have their own [work reports](#workReport) approved immediately without peer [verification](#verification), and may reduce or stop receiving verification requests. These preferences are set on the user profile. Other users can still challenge auto-approved reports by opening a dispute.

**<a name="nace"></a>NACE**

Every work report must contain information about the type of work performed. NEO uses the [NACE](https://en.wikipedia.org/wiki/Statistical_Classification_of_Economic_Activities_in_the_European_Community) work types classification scheme. A proper NACE class is aassigned to a work by the user when filing a work report. The main reason why reports must have this information is to aid the verification process. Rovas uses the NACE code to allocate a given work report for verification to the users, who have filed work reports with the same or similar NACE classification, on order to ensure the verification process is efficient. Assigning NACE information to work reports also makes finding workers with the desired type and level of expertise easy.

**<a name="dispute"></a>Dispute**

Dispute is a procedure, where one NEO user can accuse another for violating the NEO rules. The violation might concern a work report filed for work that was not performed, or one that generates product or service impossible to freely consume or buy with chrons. After filing a dispute, both parties solicit support from other users who vote for their candidate. After a set period of time, the cummulative merit scores of the supporters of both parties are tallied and the two numbers compared. The party with more merits wins. A more detailed description of the process can be found on the [Disputes page](/disputes).

**<a id="projectMEalloc" name="projectMEalloc"></a>Reward merit allocation**

A [project](#project) owner can specify how the rewards paid to the project will be allocated. Using the Reward merit allocation field of the project form, they can either leave it blank, or set a number between 0 and 100. The number indicates the part of the amount paid to the project (in percent), which will be converted to [merits](#merit). The remaining part of the payment is sent in [chrons,](#chron) to the account of the owner, or to the beneficiary of the project. An empty value sets the merit part of the reward to 100%.
